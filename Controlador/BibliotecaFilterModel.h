#ifndef BIBLIOTECAFILTERMODEL_H
#define BIBLIOTECAFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QObject>

class BibliotecaFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)

public:
    explicit BibliotecaFilterModel(QObject *parent = nullptr);

    // Q_PROPERTY READ/WRITE
    QString filterString() const;
    void setFilterString(const QString &newFilterString);

signals:
    void filterStringChanged();

protected:
    // **FUNCIÓN CLAVE PARA EL FILTRADO**
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_filterString;
};

#endif // BIBLITECAFILTERMODEL_H
