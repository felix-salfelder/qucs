/**************************************************************************
                               qucslib.cpp
                              -------------
    begin                : Sat May 28 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QMenu>
#include <QVBoxLayout>
#include <QListWidget>
#include <QtDebug>
#include <QGroupBox>
#include <QTextStream>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QComboBox>
#include <QClipboard>
#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QRegExp>
#include <QCloseEvent>
#include <QPixmap>
#include <QWidget>
#include <QBrush>

#include "qucslib.h"
#include "qucslib_common.h"
#include "librarydialog.h"
#include "displaydialog.h"
#include "symbolwidget.h"
#include "searchdialog.h"


/* Constructor setups the GUI. */
QucsLib::QucsLib()
{
    // set application icon
    setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
    setWindowTitle("Qucs Library Tool " PACKAGE_VERSION);

    // create file menu
    fileMenu = new QMenu(tr("&File"));

    QAction * manageLib =new QAction (tr("Manage User &Libraries..."), this);
    manageLib->setShortcut(Qt::CTRL+Qt::Key_M);
    connect(manageLib, SIGNAL(activated()), SLOT(slotManageLib()));

    QAction * fileQuit = new QAction(tr("&Quit"), this);
    fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

    fileMenu->addAction(manageLib);
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuit);


    // create help menu
    helpMenu = new QMenu(tr("&Help"));

    QAction * helpHelp = new QAction(tr("&Help"), this);
    helpHelp->setShortcut(Qt::Key_F1);
    helpMenu->addAction(helpHelp);
    connect(helpHelp, SIGNAL(activated()), SLOT(slotHelp()));

    QAction * helpAbout = new QAction(tr("About"), this);
    helpMenu->addAction(helpAbout);
    connect(helpAbout, SIGNAL(activated()), SLOT(slotAbout()));

    // setup menu bar
    menuBar()->addMenu(fileMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    // main box
    QWidget *main = new QWidget(this);
    setCentralWidget(main);
    all = new QVBoxLayout();
    main->setLayout(all);
    all->setSpacing (0);
    all->setMargin (0);


    // library and component choice
    QGroupBox *LibGroup = new QGroupBox(tr("Component Selection"));
    QVBoxLayout *LibGroupLayout = new QVBoxLayout();
    Library = new QComboBox();
    connect(Library, SIGNAL(activated(int)), SLOT(slotSelectLibrary(int)));
    CompList = new QListWidget();
    connect(CompList, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotShowComponent(QListWidgetItem*)));
    connect(CompList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(slotShowComponent(QListWidgetItem*)));
    QPushButton *SearchButton = new QPushButton (tr("Search..."));
    connect(SearchButton, SIGNAL(clicked()), SLOT(slotSearchComponent()));

    LibGroupLayout->addWidget(Library);
    LibGroupLayout->addWidget(CompList);
    LibGroupLayout->addWidget(SearchButton);

    LibGroup->setLayout(LibGroupLayout);


    QGroupBox *CompGroup = new QGroupBox(tr("Component"));
    QVBoxLayout *CompGroupLayout = new QVBoxLayout();
    CompDescr = new QTextEdit();
    CompDescr->setReadOnly(true);
    CompDescr->setWordWrapMode(QTextOption::NoWrap);


    Symbol = new SymbolWidget();

    QGroupBox *ButtonGroup = new QGroupBox();
    QHBoxLayout *ButtonGroupLayout = new QHBoxLayout();

    QPushButton * CopyButton = new QPushButton (tr("Copy to clipboard"));
    connect(CopyButton, SIGNAL(clicked()), SLOT(slotCopyToClipBoard()));
    QPushButton * ShowButton = new QPushButton (tr("Show Model"));
    connect(ShowButton, SIGNAL(clicked()), SLOT(slotShowModel()));

    ButtonGroupLayout->addWidget(CopyButton);
    ButtonGroupLayout->addWidget(ShowButton);
    ButtonGroup->setLayout(ButtonGroupLayout);


    CompGroupLayout->addWidget(CompDescr);
    CompGroupLayout->addWidget(Symbol);
    CompGroupLayout->addWidget(ButtonGroup);
    CompGroup->setLayout(CompGroupLayout);


    // main layout
    QWidget *h = new QWidget();
    QHBoxLayout *lh = new QHBoxLayout();
    lh->addWidget(LibGroup);
    lh->addWidget(CompGroup);
    h->setLayout(lh);
    all->addWidget(h);

    putLibrariesIntoCombobox();

}

/* Destructor destroys the application. */
QucsLib::~QucsLib()
{
}

// ----------------------------------------------------
// Put all available libraries into ComboBox.
void QucsLib::putLibrariesIntoCombobox()
{

    Library->clear();

    UserLibCount = 0;
    QStringList LibFiles;
    QStringList::iterator it;
    if(UserLibDir.cd("."))   // user library directory exists ?
    {
        //LibFiles = UserLibDir.entryList("*.lib", QDir::Files, QDir::Name);
        LibFiles = UserLibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
        UserLibCount = LibFiles.count();

        for(it = LibFiles.begin(); it != LibFiles.end(); it++)
        {
            Library->addItem(QPixmap(":/bitmaps/home.png"), (*it).left((*it).length()-4));
        }
    }


    // add a separator to distinguish between user libraries and system libs
    Library->insertSeparator(LibFiles.size());

    QDir LibDir(QucsSettings.LibDir);
    LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

    for(it = LibFiles.begin(); it != LibFiles.end(); it++)
        Library->addItem(QPixmap(":/bitmaps/big.qucs.xpm"), (*it).left((*it).length()-4));

    slotSelectLibrary(0);
}

// ----------------------------------------------------
void QucsLib::slotAbout()
{
    QMessageBox::about(this, tr("About..."),
                       "QucsLib Version " PACKAGE_VERSION "\n"+
                       tr("Library Manager for Qucs\n")+
                       tr("Copyright (C) 2005 by Michael Margraf\n")+
                       "\nThis is free software; see the source for copying conditions."
                       "\nThere is NO warranty; not even for MERCHANTABILITY or "
                       "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

// ----------------------------------------------------
void QucsLib::slotQuit()
{
    int tmp;
    tmp = x();		// call size and position function in order to ...
    tmp = y();		// ... set them correctly before closing the ...
    tmp = width();	// dialog !!!  Otherwise the frame of the window ...
    tmp = height();	// will not be recognized (a X11 problem).

    qApp->quit();
}

// ----------------------------------------------------
void QucsLib::closeEvent(QCloseEvent *Event)
{
    int tmp;
    tmp = x();		// call size and position function in order to ...
    tmp = y();		// ... set them correctly before closing the ...
    tmp = width();	// dialog !!!  Otherwise the frame of the window ...
    tmp = height();	// will not be recognized (a X11 problem).

    Event->accept();
}

// ----------------------------------------------------
void QucsLib::slotManageLib()
{
    (new LibraryDialog(this))->exec();
    putLibrariesIntoCombobox();
}

// ----------------------------------------------------
void QucsLib::slotHelp()
{
    QMessageBox helpBox;
    helpBox.setWindowTitle(tr("QucsLib Help"));
    helpBox.setText(tr("QucsLib is a program to manage Qucs component libraries. "
                       "On the left side of the application window the available "
                       "libraries can be browsed to find the wanted component. "
                       "By clicking on the component name its description can be "
                       "seen on the right side. The selected component is "
                       "transported to the Qucs application by clicking on the "
                       "button \"Copy to Clipboard\". Being back in the schematic "
                       "window the component can be inserted by pressing CTRL-V "
                       " (paste from clipboard).") + "\n" +
                    tr("A more comfortable way: The component can also be placed "
                       "onto the schematic by using Drag n'Drop."));
    helpBox.exec();
}

// ----------------------------------------------------
void QucsLib::slotCopyToClipBoard()
{
    QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
    s += "<Components>\n  " +
         Symbol->theModel() +
         "\n</Components>\n";

    // put resulting schematic into clipboard
    QClipboard *cb = QApplication::clipboard();
    cb->setText(s);
}

// ----------------------------------------------------
void QucsLib::slotShowModel()
{
    DisplayDialog *d = new DisplayDialog(this, Symbol->ModelString, Symbol->VHDLModelString, Symbol->VerilogModelString);
    d->setWindowTitle(tr("Model"));
    d->resize(500, 150);
    d->show();
}

// ----------------------------------------------------
void QucsLib::slotSelectLibrary(int Index)
{
    int End;

    End = Library->count ()-1;

    if(Library->itemText (End) == tr("Search result"))
    {
        if(Index < End)
        {
            // if search result still there -> remove it
            Library->removeItem (End);
        }
        else
        {
            return;
        }
    }

    if(Library->itemText (0) == "")
    {
        Library->removeItem (0);
    }
    CompList->clear ();
    LibraryComps.clear ();
    DefaultSymbol = "";

    QString filename;

    if(Index < UserLibCount)  // Is it user library ?
    {
        filename = UserLibDir.absolutePath() + QDir::separator() + Library->itemText(Index) + ".lib";
    }
    else
    {
        filename = QucsSettings.LibDir + Library->itemText(Index) + ".lib";
    }

    ComponentLibrary parsedlib;

    int result = parseComponentLibrary (filename, parsedlib);

    switch (result)
    {
        case QUCS_COMP_LIB_IO_ERROR:
            QMessageBox::critical(0, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
            return;
        case QUCS_COMP_LIB_CORRUPT:
            QMessageBox::critical(0, tr("Error"), tr("Library is corrupt."));
            return;
        default:
            break;
    }

    // copy the contents of default symbol section to a string
    DefaultSymbol = parsedlib.defaultSymbol;

    // Now go through the rest of the component library, extracting each
    // component name
    for (int i = 0; i < parsedlib.components.count (); i++)
    {
        CompList->addItem (parsedlib.components[i].name);
        LibraryComps.append (parsedlib.name+'\n'+parsedlib.components[i].definition);
    }

    CompList->setCurrentRow(0); // select first item
    slotShowComponent(CompList->item(0)); // and make the corresponding component info appear as well...
}

// ----------------------------------------------------
void QucsLib::slotSearchComponent()
{
    SearchDialog *d = new SearchDialog(this);
    d->setWindowTitle(tr("Search Library Component"));
    if(d->exec() == QDialog::Accepted)
        QMessageBox::information(this, tr("Result"),
                                 tr("No appropriate component found."));
}

// ----------------------------------------------------
void QucsLib::slotShowComponent(QListWidgetItem *Item)
{

    if(!Item) return;

    //QString CompString = LibraryComps.at(CompList->index(Item));
    QString CompString = LibraryComps.at(CompList->row(Item));
    QString LibName = (CompString).section('\n', 0, 0);
    CompDescr->setText("Name: " + Item->text());
    CompDescr->append("Library: " + LibName);
    CompDescr->append("----------------------------");

    if(Library->currentIndex() < UserLibCount)
        LibName = UserLibDir.absolutePath() + QDir::separator() + LibName;

    QString content;
    if(!getSection("Description", CompString, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    CompDescr->append(content);

    if(!getSection("Model", CompString, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->ModelString = content;
    if(Symbol->ModelString.count('\n') < 2)
        Symbol->createSymbol(LibName, Item->text());

    if(!getSection("VHDLModel", CompString, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->VHDLModelString = content;

    if(!getSection("VerilogModel", CompString, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    Symbol->VerilogModelString = content;

    if(!getSection("Symbol", CompString, content))
    {
        QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
        return;
    }
    if(!content.isEmpty())
        Symbol->setSymbol(content, LibName, Item->text());
    else if(!DefaultSymbol.isEmpty())   // has library a default symbol ?
        Symbol->setSymbol(DefaultSymbol, LibName, Item->text());

}


