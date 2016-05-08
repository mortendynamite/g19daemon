/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "plugin_template.hpp"
#include "menu_icon.hpp"
#include "icon.hpp"
#include <QtCore>
#include <QCommandLineParser>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <iostream>

plugin_template::plugin_template()
{
}

plugin_template::~plugin_template()
{
}

void plugin_template::run()
{
	QCommandLineParser parser;
	bool verbose = false;
	QString name;
	QString fname;
	QFile *file;
	QTextStream *ts;
	QDataStream *ds;
	
	parser.setApplicationDescription("Helper to create a plugin for g19daemon");
	
	QCommandLineOption helpOption = parser.addHelpOption();
	
    parser.addPositionalArgument("tamplate name", QCoreApplication::translate("main", "name for the template"));
    parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));

	QCommandLineOption verboseOption(QStringList() << "v" << "verbose", QCoreApplication::translate("main", "be verbose"));
	parser.addOption(verboseOption);

	QCommandLineOption versionOption(QStringList() << "V" << "version", QCoreApplication::translate("main", "be verbose"));
	parser.addOption(versionOption);

	parser.parse(QCoreApplication::arguments());
	
	if (parser.isSet(helpOption))
	{
		parser.showHelp();
	}
	
	if (parser.isSet(versionOption))
	{
		parser.showVersion();
	}
	
	if (parser.isSet(verboseOption))
	{
		verbose = true;
	}
	
	QStringList args = parser.positionalArguments();
	
	if (args.count() != 2)
		parser.showHelp();
	
	name = args[0];
	QDir path(args[1]);
	
	if (!path.mkdir(name.toLower()))
	{
		qWarning() << QCoreApplication::translate("main", "Can't create directory ") << name.toLower();
		quit();
	}
	
	path.cd(name.toLower());
	
	file = new QFile(path.filePath(QString(name.toLower() + ".hpp")));

	if (file->open(QFile::WriteOnly | QFile::Text))
	{
		ts = new QTextStream(file);
		*ts << saveHeader(name);
		file->flush();
		file->close();
		delete ts;
		delete file;
	}
	else
	{
		qWarning() << QCoreApplication::translate("main", "Can't create ") << name.toLower() << ".hpp";
	}
	
	file = new QFile(path.filePath(QString(name.toLower() + ".cpp")));

	if (file->open(QFile::WriteOnly | QFile::Text))
	{
		ts = new QTextStream(file);
		*ts << saveCpp(name);
		file->flush();
		file->close();
		delete ts;
		delete file;
	}
	else
	{
		qWarning() << QCoreApplication::translate("main", "Can't create ") << name.toLower() << ".cpp";
	}
	
	file = new QFile(path.filePath(name.toLower() + ".qrc"));

	if (file->open(QFile::WriteOnly | QFile::Text))
	{
		ts = new QTextStream(file);
		*ts << saveQrc(name);
		file->flush();
		file->close();
		delete ts;
		delete file;
	}
	else
	{
		qWarning() << QCoreApplication::translate("main", "Can't create ") << name.toLower() << ".qrc";
	}
	
	file = new QFile(path.filePath("CMakeLists.txt"));

	if (file->open(QFile::WriteOnly | QFile::Text))
	{
		ts = new QTextStream(file);
		*ts << saveCMakeLists(name);
		file->flush();
		file->close();
		delete ts;
		delete file;
	}
	else
	{
		qWarning() << QCoreApplication::translate("main", "Can't create ") << "CMakeLists.txt";
	}
	
	if (path.mkdir("images"))
	{
		path.cd("images");

		file = new QFile(path.filePath("icon.png"));
		
		if (file->open(QFile::WriteOnly))
		{
			ds = new QDataStream(file);
			ds->writeRawData((char * ) icon_png, icon_png_len);
			file->flush();
			file->close();
			delete ds;
			delete file;
		}
		else
		{
			qWarning() << QCoreApplication::translate("main", "Can't create ") << "CMakeLists.txt";
		}

		file = new QFile(path.filePath("menu_icon.png"));
		
		if (file->open(QFile::WriteOnly))
		{
			ds = new QDataStream(file);
			ds->writeRawData((char * ) menu_icon_png, menu_icon_png_len);
			file->flush();
			file->close();
			delete ds;
			delete file;
		}
		else
		{
			qWarning() << QCoreApplication::translate("main", "Can't create ") << "CMakeLists.txt";
		}
		
	}
	quit();
}

// call this routine to quit the application
void plugin_template::quit()
{
	// you can do some cleanup here
    // then do emit finished to signal CoreApplication to quit
	emit finished();
}

// shortly after quit is called the CoreApplication will signal this routine
// this is a good place to delete any objects that were created in the
// constructor and/or to stop any threads
void plugin_template::aboutToQuitApp()
{
}

QString plugin_template::saveCMakeLists(const QString &name)
{
	QString str;

	str += "cmake_minimum_required(VERSION 3.1)\n";
	str += "\n";
	str += "project("+ name.toLower() + ")\n";
	str += "\n";
	str += "set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} \"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/\")\n";
	str += "\n";
	str += "find_package(Qt5Core)\n";
	str += "find_package(Qt5Gui)\n";
	str += "\n";
	str += "include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})\n";
	str += "\n";
	str += "# Instruct CMake to run moc automatically when needed.\n";
	str += "set(CMAKE_AUTOMOC ON)\n";
	str += "\n";
	str += "set(CMAKE_CXX_FLAGS_DEBUG \"${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb\")\n";
	str += "\n";
	str += "set(" + name.toLower() + "_SRC\n";
	str += "  " + name.toLower() + ".cpp\n";
	str += ")\n";
	str += "\n";
	str += "qt5_add_resources(" + name.toLower() + "_QRC " + name.toLower() + ".qrc)\n";
	str += "\n";
	str += "add_library(" + name.toLower() + " SHARED ${" + name.toLower() + "_SRC} ${" + name.toLower() + "_QRC})\n";
	str += "target_link_libraries(" + name.toLower() + " ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES})\n";
	str += "\n";
	str += "qt5_use_modules(" + name.toLower() + " Core Gui)\n";
	str += "\n";
	str += "install(TARGETS " + name.toLower() + " LIBRARY DESTINATION ${PLUGINS_DIR})\n";

	return str;
}

QString plugin_template::saveCpp(const QString &name)
{
	QString str;
	
	str += "/*\n";
	str += " * <one line to give the program's name and a brief idea of what it does.>\n";
	str += " * Copyright (C) 2014  <copyright holder> <email>\n";
	str += " *\n";
	str += " * This program is free software: you can redistribute it and/or modify\n";
	str += " * it under the terms of the GNU General Public License as published by\n";
	str += " * the Free Software Foundation, either version 3 of the License, or\n";
	str += " * (at your option) any later version.\n";
	str += " *\n";
	str += " * This program is distributed in the hope that it will be useful,\n";
	str += " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	str += " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
	str += " * GNU General Public License for more details.\n";
	str += " *\n";
	str += " * You should have received a copy of the GNU General Public License\n";
	str += " * along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";
	str += " *\n";
	str += " */\n";
	str += "\n";
	str += "#include \"" + name.toLower() + ".hpp\"\n";
	str += "#include \"../../g19daemon.hpp\"\n";
	str += "#include \"../../gscreen.hpp\"\n";
	str += "#include <QtCore>\n";
	str += "#include <QImage>\n";
	str += "#include <QPainter>\n";
	str += "#include <QString>\n";
	str += "\n";
	str += "" + name + "::" + name + "()\n";
	str += "{\n";
	str += "	Q_INIT_RESOURCE(" + name.toLower() + ");\n";
	str += "\n";
	str += "	isActive = false;\n";
	str += "	screen = new gScreen(QImage(\":/" + name.toLower() + "/icon.png\"), tr(\"Hello World\"));\n";
	str += "}\n";
	str += "\n";
	str += "" + name + "::~" + name + "()\n";
	str += "{\n";
	str += "	delete screen;\n";
	str += "}\n";
	str += "\n";
	str += "QString " + name + "::getName()\n";
	str += "{\n";
	str += "    return tr(\"Hello World\");\n";
	str += "}\n";
	str += "\n";
	str += "void " + name + "::LKeys(int keys)\n";
	str += "{\n";
	str += "}\n";
	str += "\n";
	str += "void " + name + "::setActive(bool active)\n";
	str += "{\n";
	str += "	isActive = active;\n";
	str += "\n";
	str += "	if (active)\n";
	str += "	{\n";
	str += "		paint();\n";
	str += "	}\n";
	str += "}\n";
	str += "\n";
	str += "void " + name + "::paint()\n";
	str += "{\n";
	str += "	QPainter *p;\n";
	str += "\n";
	str += "	if (!isActive)\n";
	str += "		return;\n";
	str += "\n";
	str += "	p = screen->Begin();\n";
	str += "	p->drawText(0, 0, 320, 206, Qt::AlignCenter, tr(\"Hello World!\"));\n";
	str += "	screen->End();\n";
	str += "	emit doAction(displayScreen, screen);\n";
	str += "}\n";
	str += "\n";
	str += "bool " + name + "::isPopup()\n";
	str += "{\n";
	str += "	return false;\n";
	str += "}\n";
	str += "\n";
	str += "QObject * " + name + "::getQObject()\n";
	str += "{\n";
	str += "	return this;\n";
	str += "}\n";
	str += "\n";
	str += "QImage " + name + "::getIcon()\n";
	str += "{\n";
	str += "	return QImage(\":/" + name.toLower() + "/menu_icon.png\");\n";
	str += "}\n";

	return str;
}

QString plugin_template::saveHeader(const QString &name)
{
	QString str;

	str += "/*\n";
	str += " * <one line to give the program's name and a brief idea of what it does.>\n";
	str += " * Copyright (C) 2014  <copyright holder> <email>\n";
	str += " *\n";
	str += " * This program is free software: you can redistribute it and/or modify\n";
	str += " * it under the terms of the GNU General Public License as published by\n";
	str += " * the Free Software Foundation, either version 3 of the License, or\n";
	str += " * (at your option) any later version.\n";
	str += " *\n";
	str += " * This program is distributed in the hope that it will be useful,\n";
	str += " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	str += " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
	str += " * GNU General Public License for more details.\n";
	str += " *\n";
	str += " * You should have received a copy of the GNU General Public License\n";
	str += " * along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";
	str += " *\n";
	str += " */\n";
	str += "\n";
	str += "#ifndef " + name.toUpper() + "_H\n";
	str += "#define " + name.toUpper() + "_H\n";
	str += "\n";
	str += "#include <QtCore>\n";
	str += "#include <QtPlugin>\n";
	str += "#include \"../../plugininterface.h\"\n";
	str += "#include \"../../gscreen.h\"\n";
	str += "\n";
	str += "class " + name + " : public QObject, public PluginInterface\n";
	str += "{\n";
	str += "	Q_OBJECT\n";
	str += "    Q_INTERFACES(PluginInterface)\n";
	str += "\n";
	str += "	Q_PLUGIN_METADATA(IID \"your-string-here\" FILE \"\")\n";
	str += "	\n";
	str += "	public:\n";
	str += "		" + name + "();\n";
	str += "		~" + name + "();\n";
	str += "		void LKeys(int keys);\n";
	str += "		QString getName();\n";
	str += "		QImage getIcon();\n";
	str += "		void setActive(bool active);\n";
	str += "		bool isPopup();\n";
	str += "		QObject *getQObject();\n";
	str += "	\n";
	str += "	private:\n";
	str += "		gScreen *screen;\n";
	str += "		bool isActive;\n";
	str += "		void paint();\n";
	str += "\n";
	str += "	signals:\n";
	str += "		void doAction(gAction action, void *data);			// Signal to draw img on screen\n";
	str += "		\n";
	str += "};\n";
	str += "\n";
	str += "#endif // " + name.toUpper() + "_H\n";

	return str;
}

QString plugin_template::saveQrc(const QString &name)
{
	QString str;

	str += "<!DOCTYPE RCC><RCC version=\"1.0\">\n";
	str += "<qresource prefix=\"/" + name.toLower() + "\">\n";
	str += "    <file alias=\"icon.png\">images/icon.png</file>\n";
	str += "	<file alias=\"menu_icon.png\">images/menu_icon.png</file>\n";
	str += "</qresource>\n";
	str += "</RCC>\n";
	
	return str;
}
