// This file is part of the SpeedCrunch project
// Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2005-2006 Johan Thelin <e8johan@gmail.com>
// Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>
// Copyright (C) 2007-2008 Helder Correia <helder.pereira.correia@gmail.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.


#include "mainwindow.hxx"

#include "autohidelabel.hxx"
#include "constantsdock.hxx"
#include "deletevardlg.hxx"
#include "editor.hxx"
#include "functionsdock.hxx"
#include "historydock.hxx"
#include "insertfunctiondlg.hxx"
#include "insertvardlg.hxx"
#include "keypad.hxx"
#include "result.hxx"
#include "tipwidget.hxx"
#include "variablesdock.hxx"

#include "base/constants.hxx"
#include "base/evaluator.hxx"
#include "base/functions.hxx"
#include "base/settings.hxx"
#include "gui/aboutbox.hxx"
#include "math/hmath.hxx"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QTimer>
#include <QToolTip>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

#include <stdlib.h> // rand()


struct Actions
{
  QAction * clearExpression;
  QAction * clearHistory;
  QAction * degree;
  QAction * deleteAllVariables;
  QAction * deleteVariable;
  QAction * digits15;
  QAction * digits2;
  QAction * digits3;
  QAction * digits50;
  QAction * digits8;
  QAction * digitsAuto;
  QAction * editCopy;
  QAction * editCopyResult;
  QAction * editPaste;
  QAction * helpAbout;
  QAction * helpAboutQt;
  QAction * helpGotoWebsite;
  QAction * helpTipOfTheDay;
  QAction * insertFunction;
  QAction * insertVariable;
  QAction * optionAlwaysOnTop;
  QAction * optionAutoCalc;
  QAction * optionAutoCompletion;
  QAction * optionDotAuto;
  QAction * optionDotComma;
  QAction * optionDotPoint;
  QAction * optionMinimizeToTray;
  QAction * radian;
  QAction * radixCharAuto;
  QAction * radixCharComma;
  QAction * radixCharDot;
  QAction * scrollDown;
  QAction * scrollUp;
  QAction * selectExpression;
  QAction * sessionLoad;
  QAction * sessionQuit;
  QAction * sessionSave;
  QAction * showConstants;
  QAction * showFullScreen;
  QAction * showFunctions;
  QAction * showHistory;
  QAction * showKeypad;
  QAction * showMenuBar;
  QAction * showVariables;
  QAction * formatBinary;
  QAction * formatEngineering;
  QAction * formatFixed;
  QAction * formatGeneral;
  QAction * formatHexadec;
  QAction * formatOctal;
  QAction * formatScientific;
};


struct ActionGroups
{
  QActionGroup * angle;
  QActionGroup * digits;
  QActionGroup * format;
  QActionGroup * radixChar;
};


struct Menus
{
  QMenu * angle;
  QMenu * behavior;
  QMenu * decimal;
  QMenu * edit;
  QMenu * format;
  QMenu * help;
  QMenu * layout;
  QMenu * radixChar;
  QMenu * session;
  QMenu * settings;
  QMenu * trayIcon;
};


struct Widgets
{
  Editor *          editor;
  Keypad *          keypad;
  Result *          display;
  AutoHideLabel *   autoCalcLabel;
  TipWidget *       tip;
  QSystemTrayIcon * trayIcon;
};


struct Dialogs
{
  DeleteVariableDlg * deleteVariable;
  InsertFunctionDlg * insertFunction;
  InsertVariableDlg * insertVariable;
};


struct Docks
{
  ConstantsDock * constants;
  FunctionsDock * functions;
  HistoryDock *   history;
  VariablesDock * variables;
};


struct Conditions
{
  bool notifyMenuBarHidden;
  bool trayNotify;
  bool autoAns;
};


struct MainWindow::Private
{
  MainWindow *    p;
  Constants *     constants;
  Evaluator *     evaluator;
  Functions *     functions;
  Settings        settings;
  Actions         actions;
  ActionGroups    actionGroups;
  Menus           menus;
  Widgets         widgets;
  Dialogs         dialogs;
  Docks           docks;
  Conditions      conditions;

  Private();
  void loadSettings();
  void createUi();
  void createActions();
  void createActionGroups();
  void createActionShortcuts();
  void createMenus();
  void createWidgets();
  void createDocks();
  void createConnections();
  void applySettings();
  void restoreDocks();
  void restoreHistory();
  void restoreVariables();
  void saveSettings();
};


MainWindow::Private::Private()
{
  widgets.trayIcon               = 0;
  conditions.autoAns             = false;
  conditions.trayNotify          = true;
  conditions.notifyMenuBarHidden = true;
  dialogs.deleteVariable         = 0;
  dialogs.insertFunction         = 0;
  dialogs.insertVariable         = 0;
};


void MainWindow::Private::loadSettings()
{
  settings.load();
};


void MainWindow::Private::createUi()
{
  createActions();
  createActionGroups();
  createActionShortcuts();
  createMenus();
  createWidgets();
  createDocks();
  createConnections();

  p->setWindowTitle( "SpeedCrunch" );
  p->setWindowIcon( QPixmap( ":/speedcrunch.png" ) );
}


void MainWindow::Private::createActions()
{
  actions.clearExpression       = new QAction( tr("Clear E&xpression"),        p );
  actions.clearHistory          = new QAction( tr("Clear &History"),           p );
  actions.degree                = new QAction( tr("&Degree"),                  p );
  actions.deleteAllVariables    = new QAction( tr("Delete All V&ariables"),    p );
  actions.deleteVariable        = new QAction( tr("D&elete Variable..."),      p );
  actions.digits15              = new QAction( tr("&15 Decimal Digits"),       p );
  actions.digits2               = new QAction( tr("&2 Decimal Digits"),        p );
  actions.digits3               = new QAction( tr("&3 Decimal Digits"),        p );
  actions.digits50              = new QAction( tr("&50 Decimal Digits"),       p );
  actions.digits8               = new QAction( tr("&8 Decimal Digits"),        p );
  actions.digitsAuto            = new QAction( tr("&Automatic Precision"),     p );
  actions.editCopy              = new QAction( tr("&Copy"),                    p );
  actions.editCopyResult        = new QAction( tr("Copy Last &Result"),        p );
  actions.editPaste             = new QAction( tr("&Paste"),                   p );
  actions.helpAbout             = new QAction( tr("&About"),                   p );
  actions.helpAboutQt           = new QAction( tr("About &Qt"),                p );
  actions.helpGotoWebsite       = new QAction( tr("SpeedCrunch &Web Site..."), p );
  actions.helpTipOfTheDay       = new QAction( tr("&Tip of the Day"),          p );
  actions.insertFunction        = new QAction( tr("Insert &Function..."),      p );
  actions.insertVariable        = new QAction( tr("Insert &Variable..."),      p );
  actions.optionAutoCalc        = new QAction( tr("&Partial Results"),         p );
  actions.optionAutoCompletion  = new QAction( tr("Automatic &Completion"),    p );
  actions.optionAlwaysOnTop     = new QAction( tr("Stay Always On &Top"),      p );
  actions.optionMinimizeToTray  = new QAction( tr("&Minimize To System Tray"), p );
  actions.radixCharAuto         = new QAction( tr("&Locale Default"),          p );
  actions.radixCharDot          = new QAction( tr("&Dot"),                     p );
  actions.radixCharComma        = new QAction( tr("&Comma"),                   p );
  actions.radian                = new QAction( tr("&Radian"),                  p );
  actions.scrollDown            = new QAction( tr("Scroll Display Down"),      p );
  actions.scrollUp              = new QAction( tr("Scroll Display Up"),        p );
  actions.selectExpression      = new QAction( tr("&Select Expression"),       p );
  actions.sessionLoad           = new QAction( tr("&Load..."),                 p );
  actions.sessionQuit           = new QAction( tr("&Quit"),                    p );
  actions.sessionSave           = new QAction( tr("&Save..."),                 p );
  actions.showConstants         = new QAction( tr("Show &Constants"),          p );
  actions.showFullScreen        = new QAction( tr("Full &Screen Mode"),        p );
  actions.showFunctions         = new QAction( tr("Show &Functions"),          p );
  actions.showHistory           = new QAction( tr("Show &History"),            p );
  actions.showKeypad            = new QAction( tr("Show &Keypad"),             p );
  actions.showMenuBar           = new QAction( tr("Hide &Menu Bar"),           p );
  actions.showVariables         = new QAction( tr("Show &Variables"),          p );
  actions.formatBinary          = new QAction( tr("&Binary"),                  p );
  actions.formatEngineering     = new QAction( tr("&Engineering"),             p );
  actions.formatFixed           = new QAction( tr("&Fixed Decimal"),           p );
  actions.formatGeneral         = new QAction( tr("&General"),                 p );
  actions.formatHexadec         = new QAction( tr("&Hexadecimal"),             p );
  actions.formatOctal           = new QAction( tr("&Octal"),                   p );
  actions.formatScientific      = new QAction( tr("&Scientific"),              p );

  actions.degree->setCheckable              ( true );
  actions.digits15->setCheckable            ( true );
  actions.digits2->setCheckable             ( true );
  actions.digits3->setCheckable             ( true );
  actions.digits50->setCheckable            ( true );
  actions.digits8->setCheckable             ( true );
  actions.digitsAuto->setCheckable          ( true );
  actions.optionAlwaysOnTop->setCheckable   ( true );
  actions.optionAutoCalc->setCheckable      ( true );
  actions.optionAutoCompletion->setCheckable( true );
  actions.optionMinimizeToTray->setCheckable( true );
  actions.radian->setCheckable              ( true );
  actions.radixCharAuto->setCheckable       ( true );
  actions.radixCharComma->setCheckable      ( true );
  actions.radixCharDot->setCheckable        ( true );
  actions.showConstants->setCheckable       ( true );
  actions.showFullScreen->setCheckable      ( true );
  actions.showFunctions->setCheckable       ( true );
  actions.showHistory->setCheckable         ( true );
  actions.showKeypad->setCheckable          ( true );
  actions.showVariables->setCheckable       ( true );
  actions.formatBinary->setCheckable        ( true );
  actions.formatEngineering->setCheckable   ( true );
  actions.formatFixed->setCheckable         ( true );
  actions.formatGeneral->setCheckable       ( true );
  actions.formatHexadec->setCheckable       ( true );
  actions.formatOctal->setCheckable         ( true );
  actions.formatScientific->setCheckable    ( true );
}


void MainWindow::Private::createActionGroups()
{
  actionGroups.format = new QActionGroup( p );
  actionGroups.format->addAction( actions.formatBinary );
  actionGroups.format->addAction( actions.formatGeneral );
  actionGroups.format->addAction( actions.formatFixed );
  actionGroups.format->addAction( actions.formatEngineering );
  actionGroups.format->addAction( actions.formatScientific );
  actionGroups.format->addAction( actions.formatOctal );
  actionGroups.format->addAction( actions.formatHexadec );

  actionGroups.digits = new QActionGroup( p );
  actionGroups.digits->addAction( actions.digitsAuto );
  actionGroups.digits->addAction( actions.digits2 );
  actionGroups.digits->addAction( actions.digits3 );
  actionGroups.digits->addAction( actions.digits8 );
  actionGroups.digits->addAction( actions.digits15 );
  actionGroups.digits->addAction( actions.digits50 );

  actionGroups.angle = new QActionGroup( p );
  actionGroups.angle->addAction( actions.radian );
  actionGroups.angle->addAction( actions.degree );

  actionGroups.radixChar = new QActionGroup( p );
  actionGroups.radixChar->addAction( actions.radixCharAuto  );
  actionGroups.radixChar->addAction( actions.radixCharDot   );
  actionGroups.radixChar->addAction( actions.radixCharComma );
}


void MainWindow::Private::createActionShortcuts()
{
  actions.clearExpression->setShortcut (            Qt::Key_Escape   );
  actions.clearHistory->setShortcut    ( Qt::CTRL + Qt::Key_Y        );
  actions.degree->setShortcut          (            Qt::Key_F10      );
  actions.deleteVariable->setShortcut  ( Qt::CTRL + Qt::Key_D        );
  actions.editCopyResult->setShortcut  ( Qt::CTRL + Qt::Key_R        );
  actions.editCopy->setShortcut        ( Qt::CTRL + Qt::Key_C        );
  actions.editPaste->setShortcut       ( Qt::CTRL + Qt::Key_V        );
  actions.helpTipOfTheDay->setShortcut ( Qt::CTRL + Qt::Key_T        );
  actions.insertFunction->setShortcut  ( Qt::CTRL + Qt::Key_F        );
  actions.insertVariable->setShortcut  ( Qt::CTRL + Qt::Key_I        );
  actions.radian->setShortcut          (            Qt::Key_F9       );
  actions.scrollDown->setShortcut      (            Qt::Key_PageDown );
  actions.scrollUp->setShortcut        (            Qt::Key_PageUp   );
  actions.selectExpression->setShortcut( Qt::CTRL + Qt::Key_A        );
  actions.sessionLoad->setShortcut     ( Qt::CTRL + Qt::Key_L        );
  actions.sessionQuit->setShortcut     ( Qt::CTRL + Qt::Key_Q        );
  actions.sessionSave->setShortcut     ( Qt::CTRL + Qt::Key_S        );
  actions.showFullScreen->setShortcut  (            Qt::Key_F11      );
  actions.showKeypad->setShortcut      ( Qt::CTRL + Qt::Key_K        );
  actions.showMenuBar->setShortcut     ( Qt::CTRL + Qt::Key_M        );
  actions.formatBinary->setShortcut    (            Qt::Key_F5       );
  actions.formatGeneral->setShortcut   (            Qt::Key_F7       );
  actions.formatHexadec->setShortcut   (            Qt::Key_F8       );
  actions.formatOctal->setShortcut     (            Qt::Key_F6       );
}


void MainWindow::Private::createMenus()
{
  // session
  menus.session = new QMenu( tr("&Session"), p );
  p->menuBar()->addMenu( menus.session );
  menus.session->addAction( actions.sessionLoad );
  menus.session->addAction( actions.sessionSave );
  menus.session->addSeparator();
  menus.session->addAction( actions.sessionQuit );

  // edit
  menus.edit = new QMenu( tr("&Edit"), p );
  p->menuBar()->addMenu( menus.edit );
  menus.edit->addAction( actions.editCopy         );
  menus.edit->addAction( actions.editCopyResult   );
  menus.edit->addAction( actions.editPaste        );
  menus.edit->addAction( actions.selectExpression );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.insertFunction );
  menus.edit->addAction( actions.insertVariable );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.deleteVariable     );
  menus.edit->addAction( actions.deleteAllVariables );
  menus.edit->addSeparator();
  menus.edit->addAction( actions.clearExpression );
  menus.edit->addAction( actions.clearHistory    );

  // format
  menus.format = new QMenu( tr("&Format"), p );
  p->menuBar()->addMenu( menus.format );
  menus.format->addAction( actions.formatBinary );
  menus.format->addAction( actions.formatOctal );
  // settings / decimal
  menus.decimal = menus.format->addMenu( tr("Decimal") );
  menus.decimal->addAction( actions.formatGeneral );
  menus.decimal->addAction( actions.formatFixed );
  menus.decimal->addAction( actions.formatEngineering );
  menus.decimal->addAction( actions.formatScientific );
  menus.decimal->addSeparator();
  menus.decimal->addAction( actions.digitsAuto );
  menus.decimal->addAction( actions.digits2 );
  menus.decimal->addAction( actions.digits3 );
  menus.decimal->addAction( actions.digits8 );
  menus.decimal->addAction( actions.digits15 );
  menus.decimal->addAction( actions.digits50 );
  // format (continued)
  menus.format->addAction( actions.formatHexadec );

  // angle
  menus.angle = new QMenu( tr("&Angle"), p );
  p->menuBar()->addMenu( menus.angle );
  menus.angle->addAction( actions.radian );
  menus.angle->addAction( actions.degree );

  // settings
  menus.settings = new QMenu( tr("Se&ttings"), p );
  p->menuBar()->addMenu( menus.settings );
  // settings / layout
  menus.layout = menus.settings->addMenu( tr("&Layout") );
  menus.layout->addAction( actions.showKeypad );
  menus.layout->addSeparator();
  menus.layout->addAction( actions.showHistory   );
  menus.layout->addAction( actions.showFunctions );
  menus.layout->addAction( actions.showVariables );
  menus.layout->addAction( actions.showConstants );
  menus.layout->addSeparator();
  menus.layout->addAction( actions.showMenuBar );
  menus.layout->addSeparator();
  menus.layout->addAction( actions.showFullScreen );
  // settings / behavior
  menus.behavior = menus.settings->addMenu( tr("&Behavior") );
  menus.behavior->addAction( actions.optionAutoCalc       );
  menus.behavior->addAction( actions.optionAutoCompletion );
  menus.behavior->addAction( actions.optionAlwaysOnTop    );
  menus.behavior->addAction( actions.optionMinimizeToTray );
  // settings / radix character
  menus.radixChar = menus.settings->addMenu( tr("Radix &Character") );
  menus.radixChar->addAction( actions.radixCharAuto  );
  menus.radixChar->addAction( actions.radixCharDot   );
  menus.radixChar->addAction( actions.radixCharComma );

  // help
  menus.help = new QMenu( tr("&Help"), p );
  p->menuBar()->addMenu( menus.help );
  menus.help->addAction( actions.helpTipOfTheDay );
  menus.help->addAction( actions.helpGotoWebsite );
  menus.help->addSeparator();
  menus.help->addAction( actions.helpAbout );
  menus.help->addAction( actions.helpAboutQt );

  // necessary after hiding the menu bar, so shortcuts still recognized by main window
  p->addActions( p->menuBar()->actions() );
  p->addAction( actions.scrollDown );
  p->addAction( actions.scrollUp );
}


void MainWindow::Private::createWidgets()
{
  // necessary objects
  constants = new Constants( p );
  functions = new Functions( settings.angleMode, p );
  evaluator = new Evaluator( functions, settings.radixChar, p );

  // outer widget and layout
  QWidget * box = new QWidget( p );
  p->setCentralWidget( box );

  QVBoxLayout * boxLayout = new QVBoxLayout( box );
  boxLayout->setMargin( 0 );
  boxLayout->setSpacing( 0 );

  // display
  QHBoxLayout * displayLayout = new QHBoxLayout();
  displayLayout->setMargin( 5 );
  widgets.display = new Result( settings.radixChar, settings.format,
                                settings.precision, box );
  displayLayout->addWidget( widgets.display );
  boxLayout->addLayout( displayLayout );

  // editor
  QHBoxLayout * editorLayout = new QHBoxLayout();
  editorLayout->setMargin( 5 );
  widgets.editor = new Editor( evaluator, functions, constants,
                               settings.radixChar, box );
  widgets.editor->setFocus();
  widgets.editor->setStyleSheet( "QTextEdit { font: bold 16px }" );
  widgets.editor->setFixedHeight( widgets.editor->sizeHint().height() );
  editorLayout->addWidget( widgets.editor );
  boxLayout->addLayout( editorLayout );

  // keypad
  QHBoxLayout * keypadLayout = new QHBoxLayout();
  widgets.keypad = new Keypad( settings.radixChar, box );
  widgets.keypad->setFocusPolicy( Qt::NoFocus );
  widgets.keypad->hide();
  widgets.keypad->setStyleSheet( "QPushButton { background: black; font: bold;"
                                 "color: white; border-style: solid;"
                                 "border-color: #202020; border-radius: 10px;"
                                 "border-width: 2px }" );
  keypadLayout->addStretch();
  keypadLayout->addWidget( widgets.keypad );
  keypadLayout->addStretch();
  boxLayout->addLayout( keypadLayout );

  // for autocalc
  widgets.autoCalcLabel = new AutoHideLabel( p );
  widgets.autoCalcLabel->hide();

  // for tip of the day and menu bar hiding message
  widgets.tip = new TipWidget( p );
  widgets.tip->hide();
}


void MainWindow::Private::createDocks()
{
  docks.history = new HistoryDock( p );
  docks.history->setObjectName( "HistoryDock" );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.history );

  docks.functions = new FunctionsDock( functions, p );
  docks.functions->setObjectName( "FunctionsDock" );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.functions );

  docks.variables = new VariablesDock( settings.radixChar, p );
  docks.variables->setObjectName( "VariablesDock" );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.variables );

  docks.constants = new ConstantsDock( constants, settings.radixChar, p );
  docks.constants->setObjectName( "ConstantsDock" );
  p->addDockWidget( Qt::RightDockWidgetArea, docks.constants );

  p->tabifyDockWidget( docks.functions, docks.history );
  p->tabifyDockWidget( docks.functions, docks.functions );
  p->tabifyDockWidget( docks.functions, docks.variables );
  p->tabifyDockWidget( docks.functions, docks.constants );

  docks.history->hide();
  docks.functions->hide();
  docks.variables->hide();
  docks.constants->hide();
}


void MainWindow::Private::createConnections()
{
  QObject::connect( actions.clearHistory,                SIGNAL( activated()                           ), p,                     SLOT( clearHistory()                        ) );
  QObject::connect( actions.clearExpression,             SIGNAL( activated()                           ), p,                     SLOT( clearExpression()                     ) );
  QObject::connect( actions.degree,                      SIGNAL( activated()                           ), p,                     SLOT( degree()                              ) );
  QObject::connect( actions.deleteAllVariables,          SIGNAL( activated()                           ), p,                     SLOT( deleteAllVariables()                  ) );
  QObject::connect( actions.deleteVariable,              SIGNAL( activated()                           ), p,                     SLOT( deleteVariable()                      ) );
  QObject::connect( actions.digits15,                    SIGNAL( activated()                           ), p,                     SLOT( digits15()                            ) );
  QObject::connect( actions.digits2,                     SIGNAL( activated()                           ), p,                     SLOT( digits2()                             ) );
  QObject::connect( actions.digits3,                     SIGNAL( activated()                           ), p,                     SLOT( digits3()                             ) );
  QObject::connect( actions.digits50,                    SIGNAL( activated()                           ), p,                     SLOT( digits50()                            ) );
  QObject::connect( actions.digits8,                     SIGNAL( activated()                           ), p,                     SLOT( digits8()                             ) );
  QObject::connect( actions.digitsAuto,                  SIGNAL( activated()                           ), p,                     SLOT( digitsAuto()                          ) );
  QObject::connect( actions.editCopyResult,              SIGNAL( activated()                           ), p,                     SLOT( copyResult()                          ) );
  QObject::connect( actions.editCopy,                    SIGNAL( activated()                           ), widgets.editor,        SLOT( copy()                                ) );
  QObject::connect( actions.editPaste,                   SIGNAL( activated()                           ), widgets.editor,        SLOT( paste()                               ) );
  QObject::connect( actions.selectExpression,            SIGNAL( activated()                           ), p,                     SLOT( selectExpression()                    ) );
  QObject::connect( actions.helpAboutQt,                 SIGNAL( activated()                           ), p,                     SLOT( aboutQt()                             ) );
  QObject::connect( actions.helpAbout,                   SIGNAL( activated()                           ), p,                     SLOT( about()                               ) );
  QObject::connect( actions.helpGotoWebsite,             SIGNAL( activated()                           ), p,                     SLOT( gotoWebsite()                         ) );
  QObject::connect( actions.helpTipOfTheDay,             SIGNAL( activated()                           ), p,                     SLOT( showTipOfTheDay()                     ) );
  QObject::connect( actions.insertFunction,              SIGNAL( activated()                           ), p,                     SLOT( insertFunction()                      ) );
  QObject::connect( actions.insertVariable,              SIGNAL( activated()                           ), p,                     SLOT( insertVariable()                      ) );
  QObject::connect( actions.radian,                      SIGNAL( activated()                           ), p,                     SLOT( radian()                              ) );
  QObject::connect( actions.scrollDown,                  SIGNAL( activated()                           ), p,                     SLOT( scrollDown()                          ) );
  QObject::connect( actions.scrollUp,                    SIGNAL( activated()                           ), p,                     SLOT( scrollUp()                            ) );
  QObject::connect( actions.sessionLoad,                 SIGNAL( activated()                           ), p,                     SLOT( loadSession()                         ) );
  QObject::connect( actions.sessionQuit,                 SIGNAL( activated()                           ), p,                     SLOT( close()                               ) );
  QObject::connect( actions.sessionSave,                 SIGNAL( activated()                           ), p,                     SLOT( saveSession()                         ) );
  QObject::connect( actions.showConstants,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showConstants( bool )                 ) );
  QObject::connect( actions.showFullScreen,              SIGNAL( toggled( bool )                       ), p,                     SLOT( showInFullScreen( bool )              ) );
  QObject::connect( actions.showFunctions,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showFunctions( bool )                 ) );
  QObject::connect( actions.showHistory,                 SIGNAL( toggled( bool )                       ), p,                     SLOT( showHistory( bool )                   ) );
  QObject::connect( actions.showKeypad,                  SIGNAL( toggled( bool )                       ), p,                     SLOT( showKeypad( bool )                    ) );
  QObject::connect( actions.showMenuBar,                 SIGNAL( activated()                           ), p,                     SLOT( showMenuBar()                         ) );
  QObject::connect( actions.showVariables,               SIGNAL( toggled( bool )                       ), p,                     SLOT( showVariables( bool )                 ) );
  QObject::connect( actions.formatBinary,                SIGNAL( activated()                           ), p,                     SLOT( formatBinary()                        ) );
  QObject::connect( actions.formatEngineering,           SIGNAL( activated()                           ), p,                     SLOT( formatEngineering()                   ) );
  QObject::connect( actions.formatFixed,                 SIGNAL( activated()                           ), p,                     SLOT( formatFixed()                         ) );
  QObject::connect( actions.formatGeneral,               SIGNAL( activated()                           ), p,                     SLOT( formatGeneral()                       ) );
  QObject::connect( actions.formatHexadec,               SIGNAL( activated()                           ), p,                     SLOT( formatHexadec()                       ) );
  QObject::connect( actions.formatOctal,                 SIGNAL( activated()                           ), p,                     SLOT( formatOctal()                         ) );
  QObject::connect( actions.formatScientific,            SIGNAL( activated()                           ), p,                     SLOT( formatScientific()                    ) );
  QObject::connect( actions.optionAutoCalc,              SIGNAL( toggled( bool )                       ), p,                     SLOT( autoCalcToggled( bool )               ) );
  QObject::connect( actions.optionAutoCompletion,        SIGNAL( toggled( bool )                       ), p,                     SLOT( autoCompletionToggled( bool )         ) );
  QObject::connect( actions.optionAlwaysOnTop,           SIGNAL( toggled( bool )                       ), p,                     SLOT( alwaysOnTopToggled( bool )            ) );
  QObject::connect( actions.optionMinimizeToTray,        SIGNAL( toggled( bool )                       ), p,                     SLOT( minimizeToTrayToggled( bool )         ) );
  QObject::connect( actions.radixCharAuto,               SIGNAL( activated()                           ), p,                     SLOT( radixCharAutoActivated()              ) );
  QObject::connect( actions.radixCharDot,                SIGNAL( activated()                           ), p,                     SLOT( radixCharDotActivated()               ) );
  QObject::connect( actions.radixCharComma,              SIGNAL( activated()                           ), p,                     SLOT( radixCharCommaActivated()             ) );
  QObject::connect( widgets.keypad,                      SIGNAL( buttonPressed( Keypad::Button )       ), p,                     SLOT( keypadButtonPressed( Keypad::Button ) ) );
  QObject::connect( widgets.editor,                      SIGNAL( autoCalcActivated( const QString & )  ), p,                     SLOT( showAutoCalc( const QString & )       ) );
  QObject::connect( widgets.editor,                      SIGNAL( autoCalcDeactivated()                 ), p,                     SLOT( hideAutoCalc()                        ) );
  QObject::connect( widgets.editor,                      SIGNAL( returnPressed()                       ), p,                     SLOT( returnPressed()                       ) );
  QObject::connect( widgets.editor,                      SIGNAL( textChanged()                         ), p,                     SLOT( textChanged()                         ) );
  QObject::connect( docks.constants,                     SIGNAL( constantSelected( const QString & )   ), p,                     SLOT( constantSelected( const QString & )   ) );
  QObject::connect( docks.functions,                     SIGNAL( functionSelected( const QString & )   ), p,                     SLOT( functionSelected( const QString & )   ) );
  QObject::connect( docks.history,                       SIGNAL( expressionSelected( const QString & ) ), p,                     SLOT( expressionSelected( const QString & ) ) );
  QObject::connect( docks.variables,                     SIGNAL( variableSelected( const QString & )   ), p,                     SLOT( variableSelected( const QString & )   ) );
  QObject::connect( docks.constants->toggleViewAction(), SIGNAL( toggled( bool )                       ), actions.showConstants, SLOT( setChecked( bool )                    ) );
  QObject::connect( docks.functions->toggleViewAction(), SIGNAL( toggled( bool )                       ), actions.showFunctions, SLOT( setChecked( bool )                    ) );
  QObject::connect( docks.history->toggleViewAction(),   SIGNAL( toggled( bool )                       ), actions.showHistory,   SLOT( setChecked( bool )                    ) );
  QObject::connect( docks.variables->toggleViewAction(), SIGNAL( toggled( bool )                       ), actions.showVariables, SLOT( setChecked( bool )                    ) );
  QObject::connect( widgets.display,                     SIGNAL( textCopied( const QString & )         ), widgets.editor,        SLOT( paste()                               ) );
  QObject::connect( widgets.display,                     SIGNAL( textCopied( const QString & )         ), widgets.editor,        SLOT( setFocus()                            ) );
  QObject::connect( p,                                   SIGNAL( formatChanged( char )                 ), widgets.editor,        SLOT( setFormat( char )                     ) );
  QObject::connect( p,                                   SIGNAL( precisionChanged( int )               ), widgets.editor,        SLOT( setPrecision( int )                   ) );
  QObject::connect( p,                                   SIGNAL( radixCharChanged( char )              ), widgets.editor,        SLOT( setRadixChar( char )                  ) );
  QObject::connect( p,                                   SIGNAL( formatChanged( char )                 ), widgets.display,       SLOT( setFormat( char )                     ) );
  QObject::connect( p,                                   SIGNAL( precisionChanged( int )               ), widgets.display,       SLOT( setPrecision( int )                   ) );
  QObject::connect( p,                                   SIGNAL( radixCharChanged( char )              ), widgets.display,       SLOT( setRadixChar( char )                  ) );
  QObject::connect( p,                                   SIGNAL( radixCharChanged( char )              ), widgets.keypad,        SLOT( setRadixChar( char )                  ) );
  QObject::connect( p,                                   SIGNAL( radixCharChanged( char )              ), docks.constants,       SLOT( setRadixChar( char )                  ) );
  QObject::connect( p,                                   SIGNAL( radixCharChanged( char )              ), docks.variables,       SLOT( setRadixChar( char )                  ) );
  QObject::connect( p,                                   SIGNAL( radixCharChanged( char )              ), evaluator,             SLOT( setRadixChar( char )                  ) );
  QObject::connect( p,                                   SIGNAL( angleModeChanged( char )              ), functions,             SLOT( setAngleMode( char )                  ) );
}


void MainWindow::Private::applySettings()
{
  // window size
  if ( settings.mainWindowSize != QSize(0, 0) )
    p->resize( settings.mainWindowSize );

  // window state
  p->restoreState( settings.mainWindowState );

  // full screen
  actions.showFullScreen->setChecked( settings.showFullScreen );

  // always-on-top
  actions.optionAlwaysOnTop->setChecked( settings.stayAlwaysOnTop );

  // angle mode
  if ( settings.angleMode == 'r' )
    actions.radian->setChecked( true );
  else if ( settings.angleMode == 'd' )
    actions.degree->setChecked( true );

  // history
  if ( settings.saveSession )
    restoreHistory();
  else
    p->clearHistory();

  // variables
  if ( settings.saveVariables )
    restoreVariables();

  // format
  if ( settings.format == 'g' )
    actions.formatGeneral->setChecked( true );
  else if ( settings.format == 'f' )
    actions.formatFixed->setChecked( true );
  else if ( settings.format == 'n' )
    actions.formatEngineering->setChecked( true );
  else if ( settings.format == 'e' )
    actions.formatScientific->setChecked( true );
  else if ( settings.format == 'h' )
    actions.formatHexadec->setChecked( true );
  else if ( settings.format == 'o' )
    actions.formatOctal->setChecked( true );
  else if ( settings.format == 'b' )
    actions.formatBinary->setChecked( true );

  // precision
  if ( settings.precision <   0 )
    actions.digitsAuto->setChecked( true );
  else if ( settings.precision ==  2 )
    actions.digits2->setChecked( true );
  else if ( settings.precision ==  3 )
    actions.digits3->setChecked( true );
  else if ( settings.precision ==  8 )
    actions.digits8->setChecked( true );
  else if ( settings.precision == 15 )
    actions.digits15->setChecked( true );
  else if ( settings.precision == 50 )
    actions.digits50->setChecked( true );

  // radix character
  if ( settings.radixChar == 'C' )
    actions.radixCharAuto->setChecked( true );
  else if ( settings.radixChar == '.' )
    actions.radixCharDot->setChecked( true );
  else if ( settings.radixChar == ',' )
    actions.radixCharComma->setChecked( true );

  // keypad
  actions.showKeypad->setChecked( settings.showKeypad );

  // menu bar
  p->menuBar()->setVisible( settings.showMenuBar );

  // autocalc
  if ( settings.autoCalc )
    actions.optionAutoCalc->setChecked( true );
  else
    p->autoCalcToggled( false );

  // autocomplete
  if ( settings.autoComplete )
    actions.optionAutoCompletion->setChecked( true );
  else
    p->autoCompletionToggled( false );

  // minimize to system tray
  actions.optionMinimizeToTray->setChecked( settings.minimizeToTray );

  // docks
  actions.showConstants->setChecked( settings.showConstants );
  actions.showFunctions->setChecked( settings.showFunctions );
  actions.showHistory->setChecked( settings.showHistory );
  actions.showVariables->setChecked( settings.showVariables );
  restoreDocks();
}


void MainWindow::Private::saveSettings()
{
  // main window
  settings.mainWindowState = p->saveState();
  settings.mainWindowSize  = p->size();

  // history
  settings.history        = widgets.editor->history();
  settings.historyResults = widgets.editor->historyResults();

  // variables
  if ( settings.saveVariables )
  {
    settings.variables.clear();
    QVector<Variable> vars = evaluator->variables();
    for ( int i = 0; i < vars.count(); i++ )
    {
      QString name = vars[i].name;
      char * value = HMath::formatScientific( vars[i].value, DECPRECISION );
      settings.variables.append( QString( "%1=%2" ).arg( name ).arg( QString( value ) ) );
      free( value );
    }
  }

  // history dock
  settings.historyDockFloating   = docks.history->isFloating();
  settings.historyDockLeft       = docks.history->x();
  settings.historyDockTop        = docks.history->y();
  settings.historyDockWidth      = docks.history->width();
  settings.historyDockHeight     = docks.history->height();
  // functions dock
  settings.functionsDockFloating = docks.functions->isFloating();
  settings.functionsDockLeft     = docks.functions->x();
  settings.functionsDockTop      = docks.functions->y();
  settings.functionsDockWidth    = docks.functions->width();
  settings.functionsDockHeight   = docks.functions->height();
  // variables dock
  settings.variablesDockFloating = docks.variables->isFloating();
  settings.variablesDockLeft     = docks.variables->x();
  settings.variablesDockTop      = docks.variables->y();
  settings.variablesDockWidth    = docks.variables->width();
  settings.variablesDockHeight   = docks.variables->height();
  // constants dock
  settings.constantsDockFloating = docks.constants->isFloating();
  settings.constantsDockLeft     = docks.constants->x();
  settings.constantsDockTop      = docks.constants->y();
  settings.constantsDockWidth    = docks.constants->width();
  settings.constantsDockHeight   = docks.constants->height();

  settings.save();
}


void setWidgetLayoutAccordingToLanguageDirection( QWidget * widget )
{
  if ( QLocale().language() == QLocale::Hebrew )
    widget->setLayoutDirection( Qt::RightToLeft );
  else
    widget->setLayoutDirection( Qt::LeftToRight );
}


// public

MainWindow::MainWindow() : QMainWindow(), d( new MainWindow::Private )
{
  d->p = this;

  d->loadSettings();
  d->createUi();
  d->applySettings();

  setWidgetsLayoutAccordingToLanguageDirection();

  QTimer::singleShot( 0, this, SLOT( activate() ) );
}


bool MainWindow::event( QEvent * e )
{
  if ( e->type() == QEvent::WindowStateChange )
  {
    if ( (windowState() & Qt::WindowMinimized)
         && d->settings.minimizeToTray )
      QTimer::singleShot( 100, this, SLOT( minimizeToTray() ) );
  }

  // ensure dock windows keep their state after minimize / screen switch
  //if ( d->settings.showConstants ) QTimer::singleShot( 0, d->docks.constants,
  //                                                     SLOT( show() ) );
  //if ( d->settings.showFunctions ) QTimer::singleShot( 0, d->docks.functions,
  //                                                     SLOT( show() ) );
  //if ( d->settings.showHistory   ) QTimer::singleShot( 0, d->docks.history,
  //                                                     SLOT( show() ) );
  //if ( d->settings.showVariables ) QTimer::singleShot( 0, d->docks.variables,
  //                                                     SLOT( show() ) );

  return QMainWindow::event( e );
}


MainWindow::~MainWindow()
{
  delete d;
}


// public slots

void MainWindow::about()
{
  AboutBox * aboutBox = new AboutBox( this );
  aboutBox->exec();
  delete aboutBox;
}


void MainWindow::aboutQt()
{
  QMessageBox::aboutQt( this, tr("About Qt") );
}


void MainWindow::clearHistory()
{
  d->widgets.display->clear();
  d->widgets.editor->clearHistory();
  d->docks.history->clear();
  d->settings.history.clear();
  d->settings.historyResults.clear();
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::clearExpression()
{
  d->widgets.editor->clear();
  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::copyResult()
{
  QClipboard * cb = QApplication::clipboard();
  HNumber num = d->evaluator->get( "ans" );
  char * strToCopy = HMath::format( num, d->settings.format, d->settings.precision );
  QString final( strToCopy );
  if ( d->widgets.display->radixChar() == ',' )
    final.replace( '.', ',' );
  cb->setText( final, QClipboard::Clipboard );
  free( strToCopy );
}


void MainWindow::degree()
{
  if ( d->settings.angleMode == 'd' )
    return;

  d->settings.angleMode = 'd';
  emit angleModeChanged( 'd' );
}


void MainWindow::deleteAllVariables()
{
  d->evaluator->clearVariables();
  d->docks.variables->updateList( d->evaluator );
}


void MainWindow::deleteVariable()
{
  if ( ! d->dialogs.deleteVariable )
    d->dialogs.deleteVariable = new DeleteVariableDlg( d->evaluator, this );
  else
    d->dialogs.deleteVariable->updateList();

  d->dialogs.deleteVariable->exec();
  delete d->dialogs.deleteVariable;
  d->dialogs.deleteVariable = 0;

  d->docks.variables->updateList( d->evaluator );
}


void MainWindow::digits2()
{
  setPrecision( 2 );
}


void MainWindow::digits3()
{
  setPrecision( 3 );
}


void MainWindow::digits8()
{
  setPrecision( 8 );
}


void MainWindow::digits15()
{
  setPrecision( 15 );
}


void MainWindow::digits50()
{
  setPrecision( 50 );
}


void MainWindow::digitsAuto()
{
  setPrecision( -1 );
}


void MainWindow::selectExpression()
{
  activateWindow();
  d->widgets.editor->selectAll();
  d->widgets.editor->setFocus();
}


void MainWindow::gotoWebsite()
{
  QDesktopServices::openUrl(
    QUrl( QString::fromLatin1("http://www.speedcrunch.org") ) );
}


void MainWindow::hideAutoCalc()
{
  d->widgets.autoCalcLabel->hideText();
}


void MainWindow::insertFunction()
{
  if ( ! d->dialogs.insertFunction )
    d->dialogs.insertFunction = new InsertFunctionDlg( d->functions, this );

  if ( d->dialogs.insertFunction->exec() == InsertFunctionDlg::Accepted )
  {
    QString funcName = d->dialogs.insertFunction->functionName();
    if ( ! funcName.isEmpty() )
      d->widgets.editor->insert( funcName );
  }
  delete d->dialogs.insertFunction;
  d->dialogs.insertFunction = 0;
}


void MainWindow::insertVariable()
{
  if ( ! d->dialogs.insertVariable )
    d->dialogs.insertVariable = new InsertVariableDlg( d->evaluator, this );
  else
    d->dialogs.insertVariable->updateList();

  if ( d->dialogs.insertVariable->exec() == InsertVariableDlg::Accepted )
  {
    QString varName = d->dialogs.insertVariable->variableName();
    if ( ! varName.isEmpty() )
      d->widgets.editor->insert( varName );
  }
  delete d->dialogs.insertVariable;
  d->dialogs.insertVariable = 0;
}


void MainWindow::loadSession()
{
  QString errMsg  = tr("File %1 is not a valid session");
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname   = QFileDialog::getOpenFileName( this, tr("Load Session"),
                                                  QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::ReadOnly ) )
  {
    QMessageBox::critical( this, tr("Error"),
                           tr("Can't read from file %1").arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  QString version = stream.readLine();
  if ( version != "0.10" )
  {
    QMessageBox::critical( this, tr("Error"), errMsg.arg( fname ) );
    return;
  }

  // number of calculations
  bool ok;
  int noCalcs = stream.readLine().toInt( &ok );
  if ( ok == false || noCalcs < 0 )
  {
    QMessageBox::critical( this, tr("Error"), errMsg.arg( fname ) );
    return;
  }

  // ask for merge with current session
  QString mergeMsg = tr("Merge session being loaded with current session?\n"
                        "If no, current variables and display will be "
                        "cleared.");
  QMessageBox::StandardButton but
    = QMessageBox::question( this, tr("Question"), mergeMsg,
                             QMessageBox::Yes | QMessageBox::No
                             | QMessageBox::Cancel, QMessageBox::Yes );

  if ( but == QMessageBox::Cancel )
  {
    return;
  }
  else if ( but == QMessageBox::No )
  {
    d->widgets.display->clear();
    deleteAllVariables();
    clearHistory();
  }

  // expressions and results
  for ( int i = 0; i < noCalcs; i++ )
  {
    QString exp = stream.readLine();
    QString res = stream.readLine();
    if ( exp.isNull() || res.isNull() )
    {
      QMessageBox::critical( this, tr("Error"), errMsg.arg( fname ) );
      return;
    }
    HNumber num( res.toAscii().data() );
    if ( ! num.isNan() )
      d->widgets.display->append( exp, num );
    else
      d->widgets.display->appendError( exp, res );
  }

  // variables
  int noVars = stream.readLine().toInt( &ok );
  if ( ok == false || noVars < 0 )
  {
    QMessageBox::critical( this, tr("Error"), errMsg.arg( fname ) );
    return;
  }
  for ( int i = 0; i < noVars; i++ )
  {
    QString var = stream.readLine();
    QString val = stream.readLine();
    if ( var.isNull() || val.isNull() )
    {
      QMessageBox::critical( this, tr("Error"), errMsg.arg( fname ) );
      return;
    }
    HNumber num( val.toAscii().data() );
    if (  num != HNumber::nan() )
      d->evaluator->set( var, num );
  }

  file.close();
}


void MainWindow::alwaysOnTopToggled( bool b )
{
  d->settings.stayAlwaysOnTop = b;

  QPoint cur = mapToGlobal( QPoint(0, 0) );
  if ( b )
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  else
    setWindowFlags( windowFlags() & (~ Qt::WindowStaysOnTopHint) );
  move( cur );
  show();
}


void MainWindow::autoCalcToggled( bool b )
{
  d->settings.autoCalc = b;
  d->widgets.editor->setAutoCalcEnabled( b );
}


void MainWindow::autoCompletionToggled( bool b )
{
  d->settings.autoComplete = b;
  d->widgets.editor->setAutoCompleteEnabled( b );
}


void MainWindow::minimizeToTrayToggled( bool b )
{
  if ( b && ! d->widgets.trayIcon && QSystemTrayIcon::isSystemTrayAvailable() )
  {
    d->conditions.trayNotify = true;
    d->widgets.trayIcon = new QSystemTrayIcon( this );
    d->widgets.trayIcon->setToolTip( tr("SpeedCrunch") );
    d->widgets.trayIcon->setIcon( QPixmap( ":/speedcrunch.png" ) );

    d->menus.trayIcon = new QMenu( this );
    d->menus.trayIcon->addAction( d->actions.editCopyResult );
    d->menus.trayIcon->addSeparator();
    d->menus.trayIcon->addAction( d->actions.sessionQuit    );

    d->widgets.trayIcon->setContextMenu( d->menus.trayIcon );
    QObject::connect( d->widgets.trayIcon,
      SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this,
      SLOT( trayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
  }
  else
  {
    if ( d->widgets.trayIcon )
      delete d->widgets.trayIcon;
    d->widgets.trayIcon = 0;
  }

  d->settings.minimizeToTray = b;
}


void MainWindow::radian()
{
  if ( d->settings.angleMode == 'r' )
    return;

  d->settings.angleMode = 'r';
  emit angleModeChanged( 'r' );
}


void MainWindow::saveSession()
{
  QString filters = tr( "SpeedCrunch Sessions (*.sch);;All Files (*)" );
  QString fname = QFileDialog::getSaveFileName( this, tr("Save Session"),
                                                QString::null, filters );
  if ( fname.isEmpty() )
    return;

  QFile file( fname );
  if ( ! file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::critical( this, tr("Error"),
                           tr("Can't write to file %1").arg( fname ) );
    return;
  }

  QTextStream stream( & file );

  // version of the format
  stream << "0.10" << "\n";

  // number of calculations
  stream << d->widgets.display->count() << "\n";

  // expressions and results
  stream << d->widgets.display->asText() << "\n";

  // number of variables
  int noVars = d->evaluator->variables().count();
  stream << noVars - 2 << "\n"; // exclude pi and phi

  // variables
  for ( int i = 0; i < noVars; i++ )
  {
    Variable var = d->evaluator->variables()[i];
    if ( var.name != "pi" && var.name != "phi" )
      stream << var.name << "\n" << HMath::format( var.value ) << "\n";
  }

  file.close();
}


void MainWindow::setWidgetsLayoutAccordingToLanguageDirection()
{
  // menu bar and menus
  setWidgetLayoutAccordingToLanguageDirection( menuBar()          );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.session   );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.edit      );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.format    );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.decimal   );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.settings  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.layout    );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.behavior  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.radixChar );
  //setWidgetLayoutAccordingToLanguageDirection( d->menus.theme     );
  //setWidgetLayoutAccordingToLanguageDirection( d->menus.language  );
  setWidgetLayoutAccordingToLanguageDirection( d->menus.help      );
  // tip of the day
  setWidgetLayoutAccordingToLanguageDirection( d->widgets.tip );
  // docks
  setWidgetLayoutAccordingToLanguageDirection( d->docks.constants );
  setWidgetLayoutAccordingToLanguageDirection( d->docks.functions );
  // insert function dialog
  if ( d->dialogs.insertFunction )
    setWidgetLayoutAccordingToLanguageDirection( d->dialogs.insertFunction );
  // tip of the day
  setWidgetLayoutAccordingToLanguageDirection( d->widgets.tip );

  // speedcrunch made widgets
  emit adaptToLanguageChange();
}


void MainWindow::scrollDown()
{
  QScrollBar * sb = d->widgets.display->verticalScrollBar();
  int value = sb->value() + 40;
  sb->setValue( value );
}


void MainWindow::scrollUp()
{
  QScrollBar * sb = d->widgets.display->verticalScrollBar();
  int value = sb->value() - 40;
  sb->setValue( value );
}


void MainWindow::showMenuBar()
{
  menuBar()->setVisible( ! menuBar()->isVisible() );
  d->settings.showMenuBar = menuBar()->isVisible();

  if ( ! menuBar()->isVisible() && d->conditions.notifyMenuBarHidden )
  {
    showMenuBarTip();
    d->conditions.notifyMenuBarHidden = false;
  }

  d->widgets.display->scrollEnd();
}


void MainWindow::showAutoCalc( const QString & msg )
{
  QPoint p = d->widgets.editor->mapToParent( QPoint(0, 0) );
  d->widgets.autoCalcLabel->move( p );
  d->widgets.autoCalcLabel->showText( msg );
}


void MainWindow::showConstants( bool b )
{
  d->settings.showConstants = b;
  d->docks.constants->setVisible( b );
  //docks.constants->raise();
}


void MainWindow::showInFullScreen( bool b )
{
  b ? showFullScreen() : showNormal();
  d->settings.showFullScreen = b;
}


void MainWindow::showFunctions( bool b )
{
  if ( d->settings.showFunctions != b )
  {
    d->settings.showFunctions = b;
    d->docks.functions->setVisible( b );
  }
}


void MainWindow::showHistory( bool b )
{
  d->settings.showHistory = b;
  d->docks.history->setVisible( b );
  //docks.history->raise();
}


void MainWindow::showKeypad( bool b )
{
  d->settings.showKeypad = b;
  d->widgets.keypad->setVisible( b );
  d->widgets.display->scrollEnd();
}


void MainWindow::showMenuBarTip()
{
  QString msg = tr("The menu bar is now hidden. "
                   "To make it visible again, press Ctrl+M.");

  //QPoint p = mapFromGlobal( d->widgets.display->mapToGlobal( QPoint(0, 0) ) )
  //                          += QPoint(5, 5);
  d->widgets.tip->move( 5, 10 );
  d->widgets.tip->resize( 345, d->widgets.tip->sizeHint().height() );
  d->widgets.tip->showText( msg, tr("Warning") );
}


void MainWindow::showTipOfTheDay()
{
  //QPoint p = mapFromGlobal( d->widgets.display->mapToGlobal( QPoint(0, 0) ) )
  //                          += QPoint(5, 5);
  d->widgets.tip->move( 5, 10 );
  d->widgets.tip->resize( 345, d->widgets.tip->sizeHint().height() );

  int tipNo = rand() % 4;
  QString msg;
  switch ( tipNo )
  {
    case 0:
      msg = tr("To insert a function using keyboard, use Ctrl+F shorcut. "
               "From the dialog, you can choose the function you want to "
               "insert." );
      break;
    case 1:
      msg = tr("To insert a variable using keyboard, use Ctrl+I shorcut. "
               "From the dialog, you can choose the variable you want to "
               "insert." );
      break;
    case 2:
      msg = tr("Use variable <i>pi</i> to use pi constant." );
      break;

    case 3:
      msg = tr("Use <i>;</i> (semicolon) to separate the parameters in "
               "functions." );
      break;
    default:
      break;
  }

  d->widgets.tip->showText( msg, tr("Tip of the day") );
}


void MainWindow::showVariables( bool b )
{
  d->settings.showVariables = b;
  d->docks.variables->setVisible( b );
  //docks.variables->raise();
}


void MainWindow::formatBinary()
{
  d->actionGroups.digits->setDisabled( true );
  setFormat( 'b' );
}


void MainWindow::formatEngineering()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'n' );
}


void MainWindow::formatFixed()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'f' );
}


void MainWindow::formatGeneral()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'g' );
}


void MainWindow::formatHexadec()
{
  d->actionGroups.digits->setDisabled( true );
  setFormat( 'h' );
}


void MainWindow::formatOctal()
{
  d->actionGroups.digits->setDisabled( true );
  setFormat( 'o' );
}


void MainWindow::formatScientific()
{
  d->actionGroups.digits->setEnabled( true );
  setFormat( 'e' );
}


// protected slots

void MainWindow::activate()
{
  activateWindow();
  raise();
  d->widgets.editor->setFocus();
}


void MainWindow::constantSelected( const QString & c )
{
  if ( c.isEmpty() )
    return;

  QString str( c );
  str.replace( '.', d->docks.constants->radixChar() );
  d->widgets.editor->insert( str );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::expressionSelected( const QString & e )
{
  d->widgets.editor->setText( e );
  returnPressed();

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::functionSelected( const QString & e )
{
  if ( e.isEmpty() )
    return;

  d->widgets.editor->insert( e );
  d->widgets.editor->insert( "(" );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::keypadButtonPressed( Keypad::Button b )
{
  switch ( b )
  {
    case Keypad::Key0:         d->widgets.editor->insert( "0" );     break;
    case Keypad::Key1:         d->widgets.editor->insert( "1" );     break;
    case Keypad::Key2:         d->widgets.editor->insert( "2" );     break;
    case Keypad::Key3:         d->widgets.editor->insert( "3" );     break;
    case Keypad::Key4:         d->widgets.editor->insert( "4" );     break;
    case Keypad::Key5:         d->widgets.editor->insert( "5" );     break;
    case Keypad::Key6:         d->widgets.editor->insert( "6" );     break;
    case Keypad::Key7:         d->widgets.editor->insert( "7" );     break;
    case Keypad::Key8:         d->widgets.editor->insert( "8" );     break;
    case Keypad::Key9:         d->widgets.editor->insert( "9" );     break;
    case Keypad::KeyPlus:      d->widgets.editor->insert( "+" );     break;
    case Keypad::KeyMinus:     d->widgets.editor->insert( "-" );     break;
    case Keypad::KeyTimes:     d->widgets.editor->insert( "*" );     break;
    case Keypad::KeyDivide:    d->widgets.editor->insert( "/" );     break;
    case Keypad::KeyEE:        d->widgets.editor->insert( "e" );     break;
    case Keypad::KeyLeftPar:   d->widgets.editor->insert( "(" );     break;
    case Keypad::KeyRightPar:  d->widgets.editor->insert( ")" );     break;
    case Keypad::KeyRaise:     d->widgets.editor->insert( "^" );     break;
    case Keypad::KeyPercent:   d->widgets.editor->insert( "%" );     break;
    case Keypad::KeyFactorial: d->widgets.editor->insert( "!" );     break;
    case Keypad::KeyX:         d->widgets.editor->insert( "x" );     break;
    case Keypad::KeyXEquals:   d->widgets.editor->insert( "x=" );    break;
    case Keypad::KeyPi:        d->widgets.editor->insert( "pi" );    break;
    case Keypad::KeyAns:       d->widgets.editor->insert( "ans" );   break;
    case Keypad::KeyLn:        d->widgets.editor->insert( "ln(" );   break;
    case Keypad::KeyExp:       d->widgets.editor->insert( "exp(" );  break;
    case Keypad::KeySin:       d->widgets.editor->insert( "sin(" );  break;
    case Keypad::KeyCos:       d->widgets.editor->insert( "cos(" );  break;
    case Keypad::KeyTan:       d->widgets.editor->insert( "tan(" );  break;
    case Keypad::KeyAcos:      d->widgets.editor->insert( "acos(" ); break;
    case Keypad::KeyAtan:      d->widgets.editor->insert( "atan(" ); break;
    case Keypad::KeyAsin:      d->widgets.editor->insert( "asin(" ); break;
    case Keypad::KeySqrt:      d->widgets.editor->insert( "sqrt(" ); break;

    case Keypad::KeyRadixChar:
      d->widgets.editor->insert( QString( d->widgets.keypad->radixChar() ) );
      break;
    case Keypad::KeyEquals:
      d->widgets.editor->evaluate();
      break;
    case Keypad::KeyClear:
      clearExpression();
      break;

    default: break;
  }

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );
}


void MainWindow::minimizeToTray()
{
  if ( d->widgets.trayIcon )
  {
    hide();
    d->widgets.trayIcon->show();
    if ( d->conditions.trayNotify )
      QTimer::singleShot( 500, this, SLOT( showTrayMessage() ) );
    d->conditions.trayNotify = false;
  }
}


void MainWindow::Private::restoreVariables()
{
  for ( int k = 0; k < settings.variables.count(); k++ )
  {
    evaluator->setExpression( settings.variables[k] );
    evaluator->eval();
    QStringList list = settings.variables[k].split( "=" );
    evaluator->set( list[0], HNumber( list[1].toAscii().data() ) );
  }
  docks.variables->updateList( evaluator );
}


void MainWindow::Private::restoreDocks()
{
  if ( settings.showHistory && settings.historyDockFloating
       && ! docks.history->isFloating() )
  {
    docks.history->hide();
    docks.history->setFloating( true );
    docks.history->move( settings.historyDockLeft, settings.historyDockTop );
    docks.history->resize( settings.historyDockWidth,
                           settings.historyDockHeight );
    QTimer::singleShot( 0, docks.history, SLOT( show() ));
  }

  if ( settings.showFunctions && settings.functionsDockFloating
       && ! docks.functions->isFloating() )
  {
    docks.functions->hide();
    docks.functions->setFloating( true );
    docks.functions->move( settings.functionsDockLeft,
                           settings.functionsDockTop );
    docks.functions->resize( settings.functionsDockWidth,
                             settings.functionsDockHeight );
    QTimer::singleShot( 0, docks.functions, SLOT( show() ) );
  }
  //if ( d->settings.showFunctions && d->settings.functionsDockFloating && ! d->docks.functions->isFloating() )
  //{
  //  d->docks.functions->hide();
  //  d->docks.functions->setFloating( true );
  //  d->docks.functions->move( d->settings.functionsDockLeft, d->settings.functionsDockTop );
  //  d->docks.functions->resize( d->settings.functionsDockWidth, d->settings.functionsDockHeight );
  //  d->docks.functions->setFloating( false );
  //  QTimer::singleShot( 0, d->docks.functions, SLOT( show() ) );
  //}

  if ( settings.showVariables && settings.variablesDockFloating
       && ! docks.variables->isFloating() )
  {
    docks.variables->hide();
    docks.variables->setFloating( true );
    docks.variables->move( settings.variablesDockLeft,
                           settings.variablesDockTop );
    docks.variables->resize( settings.variablesDockWidth,
                             settings.variablesDockHeight );
    QTimer::singleShot( 0, docks.variables, SLOT( show() ) );
  }

  if ( settings.showConstants && settings.constantsDockFloating
       && ! docks.constants->isFloating() )
  {
    docks.constants->hide();
    docks.constants->setFloating( true );
    docks.constants->move( settings.constantsDockLeft,
                           settings.constantsDockTop );
    docks.constants->resize( settings.constantsDockWidth,
                             settings.constantsDockHeight );
    QTimer::singleShot( 0, docks.constants, SLOT( show() ) );
  }
}


void MainWindow::Private::restoreHistory()
{
  if ( settings.historyResults.count() != settings.history.count() )
  {
    p->clearHistory();
    return;
  }

  for ( int i = 0 ; i < settings.history.count(); i++ )
  {
    const char * resultStr = settings.historyResults[i].toAscii().data();
    HNumber result( resultStr );
    if ( ! result.isNan() )
      widgets.display->append( settings.history[i], result );
    else
      widgets.display->appendError( settings.history[i], resultStr );
  }
}


void MainWindow::returnPressed()
{
  QString str = d->evaluator->autoFix( d->widgets.editor->text() );
  if ( str.isEmpty() )
    return;

  d->evaluator->setExpression( str );
  d->docks.history->setHistory( d->widgets.editor->history() );

  HNumber result = d->evaluator->evalUpdateAns();
  if ( ! d->evaluator->error().isEmpty() )
  {
    d->widgets.display->appendError( str, d->evaluator->error() );
    d->widgets.editor->appendHistory( str, d->evaluator->error() );
  }
  else
  {
    d->widgets.display->append( str, result );
    // FIXME format is inappropriate because it saves values to 20 digits only
    d->widgets.editor->appendHistory( str, HMath::format( result ) );
    d->widgets.editor->setAnsAvailable( true );
    d->docks.variables->updateList( d->evaluator );
  }

  d->widgets.editor->setText( str );
  d->widgets.editor->selectAll();
  d->widgets.editor->stopAutoCalc();
  d->widgets.editor->stopAutoComplete();
  d->conditions.autoAns = true;

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow() )
    activateWindow();
}


void MainWindow::showTrayMessage()
{
  QString msg = tr("SpeedCrunch is minimized.\nLeft click the icon to "
                   "restore it or right click for options.");
  if ( d->widgets.trayIcon )
    d->widgets.trayIcon->showMessage( QString(), msg, QSystemTrayIcon::NoIcon,
                                      4000 );
}


void MainWindow::textChanged()
{
  if ( d->conditions.autoAns )
  {
    QString expr = d->evaluator->autoFix( d->widgets.editor->text() );
    if ( expr.isEmpty() )
	return;
    Tokens tokens = d->evaluator->scan( expr );
    if ( tokens.count() == 1 )
    if (    tokens[0].asOperator() == Token::Plus
         || tokens[0].asOperator() == Token::Minus
         || tokens[0].asOperator() == Token::Asterisk
         || tokens[0].asOperator() == Token::Slash
         || tokens[0].asOperator() == Token::Caret )
     {
       d->conditions.autoAns = false;
       expr.prepend( "ans" );
       d->widgets.editor->setText( expr );
       d->widgets.editor->setCursorPosition( expr.length() );
     }
  }
}


void MainWindow::trayIconActivated( QSystemTrayIcon::ActivationReason r )
{
  if ( r == QSystemTrayIcon::Context )
  {
    d->menus.trayIcon->show();
  }
  else
  {
    showNormal();
    activateWindow();
    d->widgets.editor->setFocus();
    QTimer::singleShot( 0, d->widgets.trayIcon, SLOT( hide() ) );
  }

  // work around docks does not reappear (under KDE/Linux)
//#ifdef Q_OS_UNIX
//  if ( docks.history->isFloating() )
//  {
//    docks.history->hide();
//    QTimer::singleShot( 0, docks.history, SLOT(show()) );
//  }
//  if ( docks.functions->isFloating() )
//  {
//    docks.functions->hide();
//    QTimer::singleShot( 0, docks.functions, SLOT(show()) );
//  }
//  if ( docks.variables->isFloating() )
//  {
//    docks.variables->hide();
//    QTimer::singleShot( 0, docks.variables, SLOT(show()) );
//  }
//  if ( docks.constants->isFloating() )
//  {
//    docks.constants->hide();
//    QTimer::singleShot( 0, docks.constants, SLOT(show()) );
//  }
//#endif
}


void MainWindow::variableSelected( const QString & v )
{
  if ( v.isEmpty() )
    return;
  d->widgets.editor->insert( v );

  QTimer::singleShot( 0, d->widgets.editor, SLOT( setFocus() ) );

  if ( ! isActiveWindow () )
    activateWindow();
}


void MainWindow::radixCharAutoActivated()
{
  char c = 'C';
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}


void MainWindow::radixCharDotActivated()
{
  char c = '.';
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}


void MainWindow::radixCharCommaActivated()
{
  char c = ',';
  d->settings.radixChar = c;
  emit radixCharChanged( c );
}


// protected

void MainWindow::closeEvent( QCloseEvent * e )
{
  if ( d->widgets.trayIcon )
    d->widgets.trayIcon->hide();
  d->saveSettings();

  d->docks.constants->hide();
  d->docks.variables->hide();
  d->docks.functions->hide();
  d->docks.history->hide();
  delete d->docks.constants;
  delete d->docks.variables;
  delete d->docks.functions;
  delete d->docks.history;

  emit quitApplication();
  QMainWindow::closeEvent( e );
}


void MainWindow::setPrecision( int p )
{
  if ( d->settings.precision == p )
    return;

  d->settings.precision = p;
  emit precisionChanged( p );
}


void MainWindow::setFormat( char c )
{
  if ( d->settings.format == c )
    return;

  d->settings.format = c;
  emit formatChanged( c );
}


void MainWindow::setRadixChar( char c )
{
  if ( d->settings.radixChar == c )
    return;

  d->settings.radixChar = c;
  emit radixCharChanged( c );
}