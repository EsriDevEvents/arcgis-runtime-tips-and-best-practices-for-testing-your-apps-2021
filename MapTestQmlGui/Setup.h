#include <QQmlEngine>

class Setup : public QObject
{
  Q_OBJECT

public:
  Setup() = default;
  ~Setup() = default;

public slots:
  void qmlEngineAvailable(QQmlEngine *engine)
  {
    // set the import path for the ArcGIS Runtime
    engine->addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
  }
};
