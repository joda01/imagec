import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material 
//import QtQuick.Controls.Material 6.0

ApplicationWindow {
    visible: true

    Material.theme: Material.Dark
    Material.accent: Material.Purple


    Column {
        anchors.centerIn: parent

        CheckBox {
            text: qsTr("Button")
           // Material.accent: Material.Orange
        }

        /*RadioButton {
            text: "Small"
        }
        RadioButton {
            text: "Medium"
            checked: true
        }
        RadioButton {
            text: "Large"
        }*/
    }
}
