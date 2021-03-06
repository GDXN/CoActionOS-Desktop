/* Copyright 2011; All Rights Reserved
 * Please see http://www.coactionos.com/license.html for
 * licensing information.
 */

#include <CSdk/CFont.h>

#include "CaosInterface.h"
#include "ui_CaosInterface.h"
#include "caoslink.h"
#include <CSdk/CNotify.h>
#include <CSdk/CFont.h>
#include <QFileInfo>


CaosInterface::CaosInterface(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::CaosInterface){
  qDebug("CAOS Interface Init");
  ui->setupUi(this);
  connect(ui->fileBrowserTab, SIGNAL(runApplication(QString)), this, SLOT(runApplicationInTerminal(QString)));
  connect(ui->applicationTab, SIGNAL(runApplication(QString)), this, SLOT(runApplicationInTerminal(QString)));
  connect(ui->terminalTab, SIGNAL(installApplication()), ui->applicationTab, SLOT(installApplication()));
  connect(ui->terminalTab, SIGNAL(runApplication(QString)), this, SLOT(runApplicationInTerminal(QString)));
  connect(ui->kernelTab, SIGNAL(aboutToReset()), ui->terminalTab, SLOT(closeTerminal()), Qt::DirectConnection);

  ui->tabs->setTabText(APPLICATION_TAB_INDEX, CFont::iconApplication());
  ui->tabs->setTabToolTip(APPLICATION_TAB_INDEX, "Application");
  ui->tabs->setTabText(KERNEL_TAB_INDEX, CFont::iconKernel());
  ui->tabs->setTabToolTip(KERNEL_TAB_INDEX, "Kernel");
  ui->tabs->setTabText(FILEBROWSER_TAB_INDEX, CFont::iconFolderClose());
  ui->tabs->setTabToolTip(FILEBROWSER_TAB_INDEX, "Filebrowser");
  ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconTerminal());
  ui->tabs->setTabToolTip(TERMINAL_TAB_INDEX, "Terminal");
  ui->tabs->setTabText(MONITOR_TAB_INDEX, CFont::iconMonitor());
  ui->tabs->setTabToolTip(MONITOR_TAB_INDEX, "Monitor");
  ui->tabs->setTabText(PREFERENCES_TAB_INDEX, CFont::iconPreferences());
  ui->tabs->setTabToolTip(PREFERENCES_TAB_INDEX, "Preferences");
  ui->tabs->setTabText(ABOUT_TAB_INDEX, CFont::iconAbout());
  ui->tabs->setTabToolTip(ABOUT_TAB_INDEX, "About");
  ui->tabs->setTabText(LABEL_TAB_INDEX, "Application");
  ui->tabs->setTabEnabled(LABEL_TAB_INDEX, false);

  connect(ui->terminalTab, SIGNAL(infoAvailable()), this, SLOT(terminalInfoAvailable()));
  connect(ui->terminalTab, SIGNAL(showPreferences()), this, SLOT(showTerminalPreferences()));

  infoTimer.setInterval(1000);
  consoleInfo = false;
  terminalInfo = false;
  connect(&infoTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
  infoTimer.start();

  qDebug("CAOS Interface Init Complete");
}

CaosInterface::~CaosInterface(){
  delete ui;
}

void CaosInterface::refresh(void){

}

void CaosInterface::setLink(CLink * d){
  CLinkPtr::setLink(d);

  connect(link(), SIGNAL(linked(bool)), this, SLOT(linked(bool)), Qt::DirectConnection);
  ui->applicationTab->setLink(d);
  ui->kernelTab->setLink(d);
  ui->fileBrowserTab->setLink(d);
  ui->terminalTab->setLink(d);
  ui->monitorTab->setLink(d);
  ui->preferencesTab->setLink(d);
  ui->aboutTab->setLink(d);

  ui->tabs->setCurrentIndex( Preferences::currentTab().toInt() );
  on_tabs_currentChanged( ui->tabs->currentIndex() );
}


void CaosInterface::showPreferences(void){
  ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showKernelPreferences(void){
  ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showTerminalPreferences(void){
  ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showApplicationPreferences(void){
  ui->tabs->setCurrentIndex(PREFERENCES_TAB_INDEX);
}

void CaosInterface::showKernel(void){
  ui->tabs->setCurrentIndex(KERNEL_TAB_INDEX);
}


void CaosInterface::updateInfo(void){

  if( terminalInfo == true ){
      if( ui->tabs->tabText(TERMINAL_TAB_INDEX) == CFont::iconEdit() ){
          ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconInfoSign());
        } else {
          ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconEdit());
        }

      if( ui->tabs->currentIndex() == TERMINAL_TAB_INDEX ){
          terminalInfo = false;
          ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconEdit());
        }
    }
}


void CaosInterface::consoleInfoAvailable(void){
  consoleInfo = true;
}

void CaosInterface::terminalInfoAvailable(void){
  terminalInfo = true;
}

void  CaosInterface::checkCloseTerminal(void){
  if( ui->terminalTab->isOpen() ){
      if( ui->monitorTab->isRunningUser() == false ){
          ui->terminalTab->closeTerminal();
        }
    }
}

void  CaosInterface::checkOpenTerminal(void){
  if( ui->terminalTab->isOpen() == false ){
      if( ui->monitorTab->isRunningUser() == true ){
          ui->terminalTab->openTerminal();
        }
    }
}

void CaosInterface::on_tabs_currentChanged(int index){
  ui->monitorTab->stopTimer();
  if( index == APPLICATION_TAB_INDEX ){
      ui->tabs->setTabText(LABEL_TAB_INDEX, "Application");
      checkCloseTerminal();
    }
  if( index == KERNEL_TAB_INDEX ){
      ui->tabs->setTabText(LABEL_TAB_INDEX, "Kernel");
    }
  if( index == FILEBROWSER_TAB_INDEX){
      ui->tabs->setTabText(LABEL_TAB_INDEX, "Filesystem");
      ui->terminalTab->closeTerminal();
    }
  if( index == TERMINAL_TAB_INDEX ){
      terminalInfo = false;
      ui->tabs->setTabText(TERMINAL_TAB_INDEX, CFont::iconEdit());
      ui->tabs->setTabText(LABEL_TAB_INDEX, "Terminal");
    }
  if( index == MONITOR_TAB_INDEX ){
      if( link()->isConnected() == true ){
          ui->monitorTab->startTimer();
        }
      checkCloseTerminal();
      ui->tabs->setTabText(LABEL_TAB_INDEX, "Monitor");
    }

  if( index ==  ABOUT_TAB_INDEX ){
      ui->tabs->setTabText(LABEL_TAB_INDEX, "About");
      checkCloseTerminal();
    }

  if( index == PREFERENCES_TAB_INDEX ){
      ui->tabs->setTabText(LABEL_TAB_INDEX, "Preferences");
      checkCloseTerminal();
    }


  Preferences::setCurrenTab(QString::number(index));

}

void CaosInterface::linked(bool connected){
  ui->monitorTab->stopTimer();
  if ( connected == true ){
      if ( ui->tabs->currentIndex() == MONITOR_TAB_INDEX ){
          ui->monitorTab->startTimer();
        } else if ( ui->tabs->currentIndex() == TERMINAL_TAB_INDEX){
          qDebug("OPEN TERMINAL");
          checkOpenTerminal();
        }
      ui->tabs->setTabText(FILEBROWSER_TAB_INDEX, CFont::iconFolderOpen());

    } else {
      ui->tabs->setTabText(FILEBROWSER_TAB_INDEX, CFont::iconFolderClose());
    }
}

void CaosInterface::runApplicationInTerminal(QString path){
  int err;
  CNotify notify;
  QFileInfo info;
  int pid;

  if( path == "" ){
      path = lastApp;
      qDebug("App was %s", path.toLocal8Bit().constData());
    }

  if ( link()->isConnected() == false ){
      notify.execError("Device not Connected");
      return;
    }

  if ( path == "" ){
      if( notify.execPrompt("Nothing to Run. Use Filesystem to execute a program?") == true ){
          ui->tabs->setCurrentIndex( FILEBROWSER_TAB_INDEX );
        }
      return;
    }


  info.setFile(path);
  if( (pid = link()->isExecuting(info.fileName().toStdString())) >= 0 ){
      if( notify.execPrompt("Kill and Re-run?") == false ){
          return;
        } else {
          if( link()->killPid(pid, LINK_SIGTERM) < 0 ){
              notify.execError("Failed to Kill program");
              return;
            }
        }
    }


  ui->terminalTab->openTerminal();
  err = link()->runApp( path.toStdString() );


  if ( err >= 0 ){
      ui->tabs->setCurrentIndex( TERMINAL_TAB_INDEX );
      lastApp = path;
    } else {
      notify.execError(errorMessage());
    }

}

