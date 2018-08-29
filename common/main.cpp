/**
* Copyright 2018 GaitRehabilitation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "forms/mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>


#if(_WIN64)
#include <wrl/wrappers/corewrappers.h>
static Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
#endif

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//  QLoggingCategory::setFilterRules(QStringLiteral("qt.bluetooth* = true"));

#if(_WIN64)
  CoInitializeSecurity(
          nullptr, // TODO: "O:BAG:BAD:(A;;0x7;;;PS)(A;;0x3;;;SY)(A;;0x7;;;BA)(A;;0x3;;;AC)(A;;0x3;;;LS)(A;;0x3;;;NS)"
          -1,
          nullptr,
          nullptr,
          RPC_C_AUTHN_LEVEL_DEFAULT,
          RPC_C_IMP_LEVEL_IDENTIFY,
          NULL,
          EOAC_NONE,
          nullptr);
#endif

  QApplication app(argc, argv);
  MainWindow window;
  window.show();

  return app.exec();
}
