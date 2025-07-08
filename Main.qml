import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window




Window {
    width: 1920
    height: 1080
    visible: true
    title: "Suspicious Object Detection"

    property string imageSource: ""

    Connections {
        target: backend
        function onLogUpdated (message) {
            txtLogArea.text += message + "\n"
        }
        function onThreatStatusChanged(msg, isThreat) {
              if(isThreat) {
                  statusBar.color = "red"
                  statusText.text = "CAUTION! THREAT DETECTED."
                  statusText.color= "white"
              }
              else
              {
                  statusBar.color = "#43A047"
                  statusText.text = "No Threat Detected."
                  statusText.color = "white"
              }
          }

        // function onClearLogRequested() {
        //       txtLogArea.text = ""
        //   }
    }

    Rectangle {
        anchors.fill: parent
        color: "#D6D6D6"

        Row {
            id: topButtons
            spacing: 30
            anchors.top: parent.top
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter

            //Kamerayı Durdur Butonu
            Button {
                id: btnToggleCamera
                width: 200
                height: 60
                padding: 0
                text: "Stop Camera"

                font.pixelSize: 20
                font.bold: true

                background: Rectangle {
                    color: "#1976D2"
                    radius: 50
                }

                contentItem: Item {
                    anchors.fill: parent

                    Text {
                        text: cameraCapture.cameraRunning ? "Stop Camera" : "Start Camera"
                        anchors.centerIn: parent
                        font.pixelSize: btnToggleCamera.font.pixelSize
                        font.bold: btnToggleCamera.font.bold
                        color: "white"
                    }
                }

                onClicked: cameraCapture.toggleCamera()
            }

            // Run Detection Butonu
            Button {
                id: btnRunDetection
                text: " Run Detection"
                width: 200
                height: 60
                padding: 0

                font.pixelSize: 20
                font.bold: true

                background: Rectangle {
                    color: "#43A047"
                    radius: 50
                }
                contentItem: Item {
                    anchors.fill: parent

                Text {
                    text: btnRunDetection.text
                    anchors.centerIn: parent
                    font.pixelSize: btnRunDetection.font.pixelSize
                    font.bold: btnRunDetection.font.bold
                    color: "white"
                }

                }
                //***
                onClicked: backend.runOCRonLastFrame()
            }
        }
        // Durum Çubuğu
        Rectangle {
            id: statusBar
            width: 960
            height: 60
            color: "#43A047"  // Yeşil
            anchors.top: topButtons.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                id: statusText
                text: "No Threat Detected."
                anchors.centerIn: parent
                font.pixelSize: 24
                font.bold: true
                color: "black"
            }


        }

        // Görüntü ve log alanı
        RowLayout {
            spacing: 20
            anchors.top: statusBar.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter

            // Sol: Kamera alanı
            Rectangle {

                id: imgPreview
                width: 640
                height: 480
                color: "#B0B0B0"


                Image {
                    id: cameraView
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    cache: false
                    source: ""
                    Timer {
                        interval: 33
                        running: true
                        repeat: true
                        onTriggered: cameraView.source = "image://live/frame?" + Date.now()
                       }
                    }
                }

            // Sağ: Detection Log
            Column {
                 spacing: 17
                 height: imgPreview.height

                Rectangle {
                    width: 300
                    height: 40
                    color: "#C0C0C0"
                    radius: 5
                    Text {
                        anchors.centerIn: parent
                        text: "Detection Log"
                        font.bold: true
                    }
                }
                Rectangle {
                    width: 300
                    height: imgPreview.height - 40 - 10 - 50
                    color: "white"
                    radius: 5

                    ScrollView {
                        width: 320
                        height: imgPreview.height - 40 - 10 - 50
                        clip: true

                        TextArea {
                            id: txtLogArea
                            width: parent.width
                            height: parent.height
                            placeholderText: "Logs will appear here..."
                            readOnly: true
                            wrapMode: TextArea.Wrap

                            onTextChanged: {
                                txtLogArea.moveCursorSelection(TextArea.End)
                            }
                        }

                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                    }

                }

                Row {
                    spacing: 18

                    Button {
                        id: btnExportLog
                        text: "Export Log"
                        width: 140
                        background: Rectangle { color: "#1976D2" }
                        contentItem: Text {
                            text: btnExportLog.text
                            anchors.centerIn: parent
                            color: "white"
                        }
                         onClicked: backend.exportLog(txtLogArea.text)
                    }

                    Button {
                        id: btnClearLog
                        text: "Clear Log"
                        width: 140
                        background: Rectangle { color: "#1976D2" }
                        contentItem: Text {
                            text: btnClearLog.text
                            anchors.centerIn: parent
                            color: "white"
                        }

                        onClicked: {
                              backend.clearLog()
                              txtLogArea.text = ""
                          }
                    }
                }
            }
        }
    }

}


