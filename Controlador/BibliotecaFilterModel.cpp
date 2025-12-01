#include "BibliotecaFilterModel.h"
// Necesitamos el archivo de encabezado del modelo base para acceder a los roles
#include "Modelo/BibliotecaListModel.h"
#include <QDebug>

BibliotecaFilterModel::BibliotecaFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // Las búsquedas no distinguirán entre mayúsculas y minúsculas
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

QString BibliotecaFilterModel::filterString() const
{
    return m_filterString;
}

void BibliotecaFilterModel::setFilterString(const QString &newFilterString)
{
    if (m_filterString == newFilterString)
        return;

    m_filterString = newFilterString;
    // Llama al framework de Qt para reevaluar todas las filas
    invalidateFilter();
    emit filterStringChanged();
}

// **IMPLEMENTACIÓN AVANZADA DE LA BÚSQUEDA**
bool BibliotecaFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterString.isEmpty()) {
        return true; // Mostrar todo si no hay filtro
    }

    // 1. Obtener el índice del modelo fuente para la fila actual
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid()) {
        return false;
    }

    // 2. Obtener los datos usando los roles del modelo fuente
    // Convertimos a minúsculas para hacer la comparación (CaseInsensitive ya está seteado, pero esto lo asegura)
    QString titulo = sourceModel()->data(index, BibliotecaListModel::TituloRole).toString().toLower();
    QString autor = sourceModel()->data(index, BibliotecaListModel::AutorRole).toString().toLower();
    QString genero = sourceModel()->data(index, BibliotecaListModel::GeneroRole).toString().toLower();
    QString filter = m_filterString.toLower();

    // 3. Lógica de Filtrado (Buscar coincidencia en cualquiera de los campos)
    return titulo.contains(filter) || autor.contains(filter) || genero.contains(filter);
}
