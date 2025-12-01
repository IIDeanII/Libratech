import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls.Basic
import QtQuick.Effects 6.0
import QtQuick.Dialogs

import LibraTechTO.Modelo 1.0

Window {
    visible: true
    width: 1100
    height: 700
    title: "LibraTech - Biblioteca Virtual"

    Rectangle {
        anchors.fill: parent
        color: "#f6f7f9"

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // BARRA SUPERIOR
            Rectangle {
                Layout.fillWidth: true
                height: 72
                color: "white"
                border.color: "#e6e9ee"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 28
                    anchors.rightMargin: 28
                    spacing: 16

                    Label {
                        text: "LibraTech"
                        font.pixelSize: 26
                        font.bold: true
                        color: "#153243"
                    }

                    // BUSCADOR
                    Rectangle {
                        Layout.fillWidth: true
                        height: 44
                        radius: 22
                        color: "#f0f2f5"
                        visible: tabBar.currentIndex === 0 // Solo visible en catálogo

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 8

                            Label { text: "🔍"; font.pixelSize: 16; color: "#6b7280" }

                            TextField {
                                id: searchField
                                placeholderText: "Buscar por título, autor, género..."
                                background: null
                                Layout.fillWidth: true
                                font.pixelSize: 15
                                color: "#153243"
                                placeholderTextColor: "#6b7280"
                                onTextChanged: bibliotecaController.bibliotecaFilterModel.filterString = text
                            }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    // Botón Agregar Libro
                    Button {
                        text: "Agregar libro"
                        Layout.preferredWidth: 160
                        height: 44
                        visible: autenticacionController.isAdmin && tabBar.currentIndex === 0

                        contentItem: Label {
                            text: parent.text
                            font.pixelSize: 14
                            font.bold: true
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: parent.down ? "#0d9488" : "#14b8a6"
                            radius: 8
                        }

                        onClicked: agregarLibroDialog.open()
                    }

                    // Usuario logueado
                    Label {
                        text: "👤 " + autenticacionController.currentUserNombre
                        font.pixelSize: 14
                        color: "#4b5563"
                    }

                    // Botón Logout
                    Button {
                        text: "Cerrar sesión"
                        height: 36

                        onClicked: autenticacionController.logout()
                    }
                }
            }

            // PESTAÑAS (Catálogo / Historial / Recomendaciones)
            TabBar {
                id: tabBar
                Layout.fillWidth: true
                Layout.leftMargin: 28
                Layout.rightMargin: 28
                Layout.topMargin: 10

                TabButton {
                    text: "📚 Catálogo"
                    font.pixelSize: 15
                }

                TabButton {
                    text: "📋 Historial"
                    font.pixelSize: 15
                    visible: autenticacionController.isAdmin
                }


                TabButton {
                    text: "Recomendaciones"
                    font.pixelSize: 15
                    visible: !autenticacionController.isAdmin

                    onClicked: {
                        recomendacionController.generarRecomendaciones(autenticacionController.currentUserNombre);
                    }
                }
            }

            // CONTENIDO DE LAS PESTAÑAS
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: tabBar.currentIndex


                // PESTAÑA 1: CATÁLOGO DE LIBROS

                Item {

                    Loader {
                        anchors.fill: parent
                        sourceComponent: autenticacionController.isAdmin ? vistaListaAdmin : vistaGridUsuario
                    }

                    // Componente: Vista Grid para Usuarios Regulares
                    Component {
                        id: vistaGridUsuario

                        GridView {
                            id: gridView
                            anchors.fill: parent
                            anchors.margins: 20

                            model: bibliotecaController.bibliotecaFilterModel
                            cellWidth: 260
                            cellHeight: 420

                            clip: true
                            boundsBehavior: Flickable.StopAtBounds
                            boundsMovement: Flickable.StopAtBounds

                            delegate: Item {
                                width: gridView.cellWidth
                                height: gridView.cellHeight

                                Rectangle {
                                    id: card
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: 8
                                    width: parent.width - 24
                                    height: parent.height - 16
                                    radius: 12
                                    color: "white"
                                    border.color: "#e6e9ee"
                                    border.width: 1

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 12
                                        spacing: 8

                                        Rectangle {
                                            width: parent.width
                                            height: 200
                                            radius: 8
                                            color: "#fafafa"
                                            clip: true

                                            Image {
                                                anchors.fill: parent
                                                anchors.margins: 6
                                                source: imagenPath
                                                fillMode: Image.PreserveAspectCrop
                                                smooth: true
                                            }
                                        }

                                        Label {
                                            text: autor
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: "#153243"
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        Label {
                                            text: titulo
                                            font.pixelSize: 13
                                            color: "#4b5563"
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        Label {
                                            text: "Año: " + anioPublicacion + "  •  " + genero
                                            font.pixelSize: 12
                                            color: "#6b7280"
                                        }

                                        Rectangle {
                                            Layout.fillWidth: true
                                            height: prestado ? 50 : 0
                                            visible: prestado
                                            color: "transparent"

                                            ColumnLayout {
                                                anchors.fill: parent
                                                spacing: 4

                                                Label {
                                                    text: "Prestado: " + (fechaPrestamo !== "N/A" ? fechaPrestamo : "Sin fecha")
                                                    font.pixelSize: 11
                                                    color: "#374151"
                                                    Layout.fillWidth: true
                                                }

                                                Label {
                                                    text: "Devolución: " + (fechaDevolucionEsperada !== "N/A" ? fechaDevolucionEsperada : "Sin fecha")
                                                    font.pixelSize: 11
                                                    color: estaRetrasado ? "#dc2626" : "#374151"
                                                    font.bold: estaRetrasado
                                                    Layout.fillWidth: true
                                                }

                                                Label {
                                                    visible: estaRetrasado
                                                    text: "RETRASADO"
                                                    font.pixelSize: 10
                                                    font.bold: true
                                                    color: "#dc2626"
                                                }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6

                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 8

                                                Rectangle {
                                                    radius: 8
                                                    width: 92
                                                    height: 28
                                                    color: prestado ? (estaRetrasado ? "#fef2f2" : "#fff1f2") : "#ecfdf5"
                                                    border.color: prestado ? (estaRetrasado ? "#dc2626" : "#fecaca") : "#bbf7d0"

                                                    Label {
                                                        anchors.centerIn: parent
                                                        text: prestado ? (estaRetrasado ? "Retrasado" : "Prestado") : "Disponible"
                                                        font.pixelSize: 11
                                                        color: "#0f172a"
                                                        font.bold: estaRetrasado
                                                    }
                                                }

                                                Button {
                                                    text: prestado ? "Devolver" : "Prestar"
                                                    Layout.fillWidth: true
                                                    height: 36

                                                    onClicked: {
                                                        if (prestado) bibliotecaController.realizarDevolucion(libroId)
                                                        else bibliotecaController.realizarPrestamo(libroId)
                                                    }
                                                }
                                            }

                                            Button {
                                                text: "🗑️ Eliminar"
                                                Layout.fillWidth: true
                                                height: 32
                                                visible: autenticacionController.isAdmin

                                                contentItem: Label {
                                                    text: parent.text
                                                    font.pixelSize: 12
                                                    font.bold: true
                                                    color: "white"
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                }

                                                background: Rectangle {
                                                    color: parent.down ? "#991b1b" : "#dc2626"
                                                    radius: 6
                                                }

                                                onClicked: {
                                                    confirmarEliminarDialog.libroIdAEliminar = libroId;
                                                    confirmarEliminarDialog.libroTituloAEliminar = titulo;
                                                    confirmarEliminarDialog.open();
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Componente: Vista Lista para Administradores
                    Component {
                        id: vistaListaAdmin

                        ListView {
                            id: listViewAdmin
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 10

                            clip: true
                            boundsBehavior: Flickable.StopAtBounds
                            boundsMovement: Flickable.StopAtBounds

                            model: bibliotecaController.bibliotecaFilterModel

                            delegate: Rectangle {
                                width: listViewAdmin.width
                                height: 100
                                radius: 8
                                color: "white"
                                border.color: "#e6e9ee"
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 15
                                    spacing: 15

                                    Rectangle {
                                        width: 70
                                        height: 70
                                        radius: 6
                                        color: "#fafafa"
                                        clip: true

                                        Image {
                                            anchors.fill: parent
                                            anchors.margins: 3
                                            source: imagenPath
                                            fillMode: Image.PreserveAspectCrop
                                            smooth: true
                                        }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 4

                                        Label {
                                            text: titulo
                                            font.pixelSize: 15
                                            font.bold: true
                                            color: "#153243"
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                        }

                                        Label {
                                            text: "Autor: " + autor
                                            font.pixelSize: 13
                                            color: "#4b5563"
                                        }

                                        RowLayout {
                                            spacing: 15

                                            Label {
                                                text: "Año: " + anioPublicacion
                                                font.pixelSize: 12
                                                color: "#6b7280"
                                            }

                                            Label {
                                                text: "Género: " + genero
                                                font.pixelSize: 12
                                                color: "#6b7280"
                                            }
                                        }
                                    }

                                    Rectangle {
                                        width: 100
                                        height: 32
                                        radius: 6
                                        color: prestado ? (estaRetrasado ? "#fef2f2" : "#fff1f2") : "#ecfdf5"
                                        border.color: prestado ? (estaRetrasado ? "#dc2626" : "#fecaca") : "#bbf7d0"
                                        border.width: 1

                                        Label {
                                            anchors.centerIn: parent
                                            text: prestado ? (estaRetrasado ? "Retrasado" : "Prestado") : "Disponible"
                                            font.pixelSize: 12
                                            font.bold: estaRetrasado
                                            color: "#0f172a"
                                        }
                                    }

                                    Button {
                                        text: "🗑️"
                                        width: 40
                                        height: 40

                                        contentItem: Label {
                                            text: parent.text
                                            font.pixelSize: 18
                                            color: "white"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        background: Rectangle {
                                            color: parent.down ? "#991b1b" : "#dc2626"
                                            radius: 6
                                        }

                                        onClicked: {
                                            confirmarEliminarDialog.libroIdAEliminar = libroId;
                                            confirmarEliminarDialog.libroTituloAEliminar = titulo;
                                            confirmarEliminarDialog.open();
                                        }
                                    }
                                }
                            }

                            Label {
                                visible: listViewAdmin.count === 0
                                anchors.centerIn: parent
                                text: "No hay libros en el catálogo"
                                font.pixelSize: 16
                                color: "#9ca3af"
                            }
                        }
                    }
                }




                // PESTAÑA 2: HISTORIAL (Solo Admin)

                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        // Header del historial
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 15

                            Label {
                                text: "Historial de Préstamos y Devoluciones"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#153243"
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Actualizar"
                                height: 36
                                onClicked: historialController.cargarHistorial()
                            }

                            Button {
                                text: "Limpiar historial"
                                height: 36

                                contentItem: Label {
                                    text: parent.text
                                    font.pixelSize: 13
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                background: Rectangle {
                                    color: parent.down ? "#991b1b" : "#dc2626"
                                    radius: 6
                                }

                                onClicked: {
                                    confirmarLimpiarDialog.open()
                                }
                            }
                        }

                        // Lista del historial
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "white"
                            radius: 10
                            border.color: "#e6e9ee"
                            border.width: 1

                            ListView {
                                id: historialListView
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 8
                                clip: true

                                model: historialController.historialModel

                                delegate: Rectangle {
                                    width: historialListView.width
                                    height: 80
                                    radius: 8
                                    color: esPrestamo ? "#f0fdf4" : "#fef2f2"
                                    border.color: esPrestamo ? "#bbf7d0" : "#fecaca"
                                    border.width: 1

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 12
                                        spacing: 15

                                        // Icono
                                        Label {
                                            text: esPrestamo ? "📤" : "📥"
                                            font.pixelSize: 32
                                        }

                                        // Información del libro
                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 4

                                            Label {
                                                text: libroTitulo
                                                font.pixelSize: 15
                                                font.bold: true
                                                color: "#0f172a"
                                            }

                                            Label {
                                                text: "Autor: " + libroAutor
                                                font.pixelSize: 13
                                                color: "#4b5563"
                                            }

                                            Label {
                                                text: "Usuario: " + usuarioNombre
                                                font.pixelSize: 12
                                                color: "#6b7280"
                                            }
                                        }


                                        ColumnLayout {
                                            spacing: 4
                                            Layout.alignment: Qt.AlignRight

                                            Rectangle {
                                                width: 100
                                                height: 26
                                                radius: 6
                                                color: esPrestamo ? "#22c55e" : "#ef4444"

                                                Label {
                                                    anchors.centerIn: parent
                                                    text: accion
                                                    font.pixelSize: 12
                                                    font.bold: true
                                                    color: "white"
                                                }
                                            }

                                            Label {
                                                text: fechaFormateada
                                                font.pixelSize: 12
                                                color: "#6b7280"
                                                horizontalAlignment: Text.AlignRight
                                            }
                                        }
                                    }
                                }

                                // Mensaje si está vacío
                                Label {
                                    visible: historialListView.count === 0
                                    anchors.centerIn: parent
                                    text: "No hay registros en el historial"
                                    font.pixelSize: 16
                                    color: "#9ca3af"
                                }
                            }
                        }

                        // Estadísticas
                        Rectangle {
                            Layout.fillWidth: true
                            height: 60
                            color: "#f9fafb"
                            radius: 8
                            border.color: "#e5e7eb"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 15
                                spacing: 30

                                Label {
                                    text: "Total de registros: " + historialController.historialModel.count
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#374151"
                                }
                            }
                        }
                    }
                }


                // PESTAÑA 3: RECOMENDACIONES

                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 15

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 15

                            Label {
                                text: "✨ Recomendaciones Para Ti"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#153243"
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Actualizar"
                                height: 36
                                onClicked: {
                                    recomendacionController.generarRecomendaciones(autenticacionController.currentUserNombre);
                                }
                            }
                        }

                        Label {
                            text: "Basado en tus préstamos anteriores, te sugerimos estos libros disponibles:"
                            font.pixelSize: 14
                            color: "#6b7280"
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }


                        GridView {
                            id: recomendacionesGridView
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            model: recomendacionController.recomendacionesModel
                            cellWidth: 260
                            cellHeight: 380

                            delegate: Item {
                                width: recomendacionesGridView.cellWidth
                                height: recomendacionesGridView.cellHeight

                                Rectangle {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: 8
                                    width: parent.width - 24
                                    height: parent.height - 16
                                    radius: 12
                                    color: "white"
                                    border.color: "#fbbf24"
                                    border.width: 2


                                    Rectangle {
                                        width: 120
                                        height: 28
                                        color: "#fbbf24"
                                        radius: 6
                                        anchors.top: parent.top
                                        anchors.topMargin: -10
                                        anchors.horizontalCenter: parent.horizontalCenter

                                        Label {
                                            anchors.centerIn: parent
                                            text: "Recomendado"
                                            font.pixelSize: 11
                                            font.bold: true
                                            color: "white"
                                        }
                                    }

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 12
                                        anchors.topMargin: 20
                                        spacing: 8

                                        Rectangle {
                                            width: parent.width
                                            height: 180
                                            radius: 8
                                            color: "#fafafa"
                                            clip: true

                                            Image {
                                                anchors.fill: parent
                                                anchors.margins: 6
                                                source: imagenPath
                                                fillMode: Image.PreserveAspectCrop
                                                smooth: true
                                            }
                                        }

                                        Label {
                                            text: autor
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: "#153243"
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        Label {
                                            text: titulo
                                            font.pixelSize: 13
                                            color: "#4b5563"
                                            wrapMode: Text.WordWrap
                                            Layout.fillWidth: true
                                        }

                                        Label {
                                            text: "Año: " + anioPublicacion + "  •  " + genero
                                            font.pixelSize: 12
                                            color: "#6b7280"
                                        }

                                        Button {
                                            text: "Prestar este libro"
                                            Layout.fillWidth: true
                                            height: 40

                                            contentItem: Label {
                                                text: parent.text
                                                font.pixelSize: 13
                                                font.bold: true
                                                color: "white"
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            background: Rectangle {
                                                color: parent.down ? "#0d9488" : "#14b8a6"
                                                radius: 8
                                            }

                                            onClicked: {
                                                bibliotecaController.realizarPrestamo(libroId);
                                                tabBar.currentIndex = 0; // Volver al catálogo
                                            }
                                        }
                                    }
                                }
                            }


                            Label {
                                visible: recomendacionesGridView.count === 0
                                anchors.centerIn: parent
                                text: "No hay recomendaciones disponibles.\nRealiza algunos préstamos para obtener sugerencias personalizadas."
                                font.pixelSize: 16
                                color: "#9ca3af"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }

            }
        }
    }


        // DIÁLOGO: AGREGAR LIBRO

        Dialog {
            id: agregarLibroDialog
            title: "➕ Agregar Nuevo Libro"
            modal: true
            anchors.centerIn: parent
            width: 520
            height: 600

            standardButtons: Dialog.Ok | Dialog.Cancel

            ScrollView {
                anchors.fill: parent
                clip: true

                ColumnLayout {
                    width: parent.width
                    spacing: 15

                    Label { text: "Título *"; font.pixelSize: 13; color: "#374151"; font.bold: true }
                    TextField {
                        id: tituloInput
                        Layout.fillWidth: true
                        placeholderText: "Ej: Cien Años de Soledad"
                        font.pixelSize: 14
                    }

                    Label { text: "Autor *"; font.pixelSize: 13; color: "#374151"; font.bold: true }
                    TextField {
                        id: autorInput
                        Layout.fillWidth: true
                        placeholderText: "Ej: Gabriel García Márquez"
                        font.pixelSize: 14
                    }

                    Label { text: "Año de Publicación *"; font.pixelSize: 13; color: "#374151"; font.bold: true }
                    SpinBox {
                        id: anioInput
                        Layout.fillWidth: true
                        from: 1000
                        to: 2100
                        value: 2024
                        editable: true
                    }

                    Label { text: "Género *"; font.pixelSize: 13; color: "#374151"; font.bold: true }
                    TextField {
                        id: generoInput
                        Layout.fillWidth: true
                        placeholderText: "Ej: Novela, Ciencia Ficción, etc."
                        font.pixelSize: 14
                    }

                    Label { text: "Ruta de la Imagen (opcional)"; font.pixelSize: 13; color: "#374151"; font.bold: true }
                    TextField {
                        id: imagenInput
                        Layout.fillWidth: true
                        placeholderText: "qrc:/Vista/images/miimagen.jpg"
                        font.pixelSize: 14
                    }


                    Rectangle {
                        Layout.fillWidth: true
                        height: infoColumn.height + 20
                        color: "#eff6ff"
                        radius: 6
                        border.color: "#bfdbfe"
                        border.width: 1

                        ColumnLayout {
                            id: infoColumn
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 5

                            Label {
                                text: "Formatos de ruta válidos:"
                                font.pixelSize: 12
                                font.bold: true
                                color: "#1e40af"
                            }

                            Label {
                                text: "• qrc:/Vista/images/nombre.jpg (desde recursos)"
                                font.pixelSize: 11
                                color: "#1e3a8a"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            Label {
                                text: "• Dejar vacío para usar imagen por defecto"
                                font.pixelSize: 11
                                color: "#1e3a8a"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }
                        }
                    }

                    Label {
                        id: mensajeResultado
                        text: ""
                        font.pixelSize: 12
                        color: "#dc2626"
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }
                }
            }

            onAccepted: {
                var exito = bibliotecaController.agregarLibroDesdeUI(
                    tituloInput.text,
                    autorInput.text,
                    anioInput.value,
                    generoInput.text,
                    imagenInput.text
                );

                if (exito) {
                    tituloInput.text = "";
                    autorInput.text = "";
                    anioInput.value = 2024;
                    generoInput.text = "";
                    imagenInput.text = "";
                    mensajeResultado.text = "";
                }
            }

            onRejected: {
                mensajeResultado.text = "";
            }
        }

    // Conexión para mostrar mensajes
    Connections {
        target: bibliotecaController
        function onLibroAgregado(exito, mensaje) {
            mensajeResultado.text = mensaje;
            mensajeResultado.color = exito ? "#10b981" : "#dc2626";
        }
    }


    // DIALOGO: CONFIRMAR LIMPIAR HISTORIAL

    Dialog {
        id: confirmarLimpiarDialog
        title: "⚠️ Confirmar"
        modal: true
        anchors.centerIn: parent

        standardButtons: Dialog.Yes | Dialog.No

        Label {
            text: "¿Estás seguro de que deseas eliminar TODO el historial?\nEsta acción no se puede deshacer."
            font.pixelSize: 14
            wrapMode: Text.WordWrap
        }

        onAccepted: {
            historialController.limpiarHistorial();
        }
    }


    // DIALOGO: LOGIN

    Dialog {
        id: loginDialog
        modal: true
        width: parent.width
        height: parent.height
        visible: !autenticacionController.isLoggedIn

        background: Rectangle {
            color: "#90000000"
        }

        Rectangle {
            id: loginCard
            width: 440
            height: 480
            radius: 20
            color: "white"
            anchors.centerIn: parent
            border.color: "#e5e7eb"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 25

                Item { Layout.fillHeight: true; Layout.preferredHeight: 0 }

                Label {
                    text: "📚"
                    font.pixelSize: 48
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: "Bienvenido a LibraTech"
                    font.pixelSize: 22
                    font.bold: true
                    color: "#153243"
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: "Inicia sesión para continuar."
                    font.pixelSize: 15
                    color: "#6b7280"
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    id: errorLabel
                    text: ""
                    color: "#dc2626"
                    font.pixelSize: 13
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight

                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                    visible: text.length > 0
                }

                Label { text: "Email"; font.pixelSize: 13; color: "#153243" }
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    radius: 8
                    border.color: "#e5e7eb"
                    border.width: 1

                    TextField {
                        id: emailInput
                        placeholderText: "tucorreo@ejemplo.com"
                        anchors.fill: parent
                        anchors.margins: 5
                        background: null
                        font.pixelSize: 13
                        verticalAlignment: Text.AlignVCenter
                        color: "#1f2937"
                        placeholderTextColor: "#6b7280"
                    }
                }

                Label { text: "Contraseña"; font.pixelSize: 13; color: "#153243" }
                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    radius: 8
                    border.color: "#e5e7eb"
                    border.width: 1

                    TextField {
                        id: passwordInput
                        placeholderText: "Ingresa tu contraseña"
                        echoMode: TextInput.Password
                        anchors.fill: parent
                        anchors.margins: 5
                        background: null
                        font.pixelSize: 13
                        onAccepted: loginButton.clicked()
                        verticalAlignment: Text.AlignVCenter
                        color: "#1f2937"
                        placeholderTextColor: "#6b7280"
                    }
                }



                Button {
                    id: loginButton
                    text: "Iniciar Sesión"
                    Layout.fillWidth: true
                    height: 48

                    contentItem: Label {
                        text: loginButton.text
                        font.bold: true
                        font.pixelSize: 16
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: loginButton.down ? "#991b1b" : "#dc2626"
                        radius: 8
                    }

                    onClicked: {
                        if (autenticacionController.login(emailInput.text, passwordInput.text)) {
                            errorLabel.text = "";
                        } else {
                            errorLabel.text = "Error: Email o contraseña incorrectos.";
                        }
                    }
                }

                Item { Layout.fillHeight: true; Layout.preferredHeight: 0 }
            }

        }
    }


    // DIÁLOGO: CONFIRMAR ELIMINAR LIBRO

    Dialog {
        id: confirmarEliminarDialog
        title: "Eliminar Libro"
        modal: true
        anchors.centerIn: parent
        width: 400

        property var libroIdAEliminar: null
        property string libroTituloAEliminar: ""

        standardButtons: Dialog.Yes | Dialog.No

        ColumnLayout {
            anchors.fill: parent
            spacing: 15

            Label {
                text: "¿Estás seguro de que deseas eliminar este libro?"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Rectangle {
                Layout.fillWidth: true
                height: 60
                color: "#fef2f2"
                radius: 8
                border.color: "#fecaca"
                border.width: 1

                Label {
                    anchors.centerIn: parent
                    text: confirmarEliminarDialog.libroTituloAEliminar
                    font.pixelSize: 15
                    font.bold: true
                    color: "#dc2626"
                    wrapMode: Text.WordWrap
                    width: parent.width - 20
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Label {
                text: "Esta acción no se puede deshacer."
                font.pixelSize: 12
                color: "#dc2626"
                font.bold: true
            }
        }

        onAccepted: {
            if (confirmarEliminarDialog.libroIdAEliminar) {
                bibliotecaController.eliminarLibro(confirmarEliminarDialog.libroIdAEliminar);
            }
        }
    }

}
