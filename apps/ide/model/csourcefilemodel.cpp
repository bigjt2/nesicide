
#include "model/csourcefilemodel.h"
#include "model/projectsearcher.h"
#include "cnesicideproject.h"

CSourceFileModel::CSourceFileModel()
   : m_pProject(NULL)
{

}

void CSourceFileModel::setProject(CNesicideProject *project)
{
   m_pProject = project;
   emit reset();
}

QUuid CSourceFileModel::newSourceFile(const QString& path)
{
   if (m_pProject == NULL || path.isNull())
      return QUuid();

   CSourceItem *file = m_pProject->getProject()->getSources()->addSourceFile(path);
   emit sourceFileAdded(file->uuid());
   return file->uuid();
}

void CSourceFileModel::removeSourceFile(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   // Make sure item has correct type before doing anything.
   CSourceItem* item = ProjectSearcher::findItemByUuid<CSourceItem>(m_pProject, uuid);
   if (item == NULL)
      return;

   m_pProject->getProject()->getSources()->removeChild(item);
   delete item;
   emit sourceFileRemoved(uuid);
}

QList<QUuid> CSourceFileModel::getUuids() const
{
   if (m_pProject == NULL)
      return QList<QUuid>();

   QList<CSourceItem*> files = ProjectSearcher::findItemsOfType<CSourceItem>(m_pProject);

   QList<QUuid> uuids;
   foreach(CSourceItem* file, files)
   {
      uuids.append(file->uuid());
   }
   return uuids;
}

QString CSourceFileModel::getSourceCode(const QUuid &uuid) const
{
   if (m_pProject == NULL)
      return QString();

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pProject, uuid);
   return file != NULL ? file->sourceCode() : QString();
}

QString CSourceFileModel::getRelativePath(const QUuid &uuid) const
{
   if (m_pProject == NULL)
      return QString();

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pProject, uuid);
   return file != NULL ? file->path() : QString();
}

QString CSourceFileModel::getFileName(const QUuid &uuid) const
{
   if (m_pProject == NULL)
      return QString();

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pProject, uuid);
   return file != NULL ? file->caption() : QString();
}

void CSourceFileModel::setSourceCode(const QUuid &uuid, const QString &source)
{
   if (m_pProject == NULL)
      return;

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pProject, uuid);
   if (file == NULL)
      return;

   file->setSourceCode(source);
   emit sourceFileChanged(uuid);
}

void CSourceFileModel::setRelativePath(const QUuid &uuid, const QString &path)
{
   if (m_pProject == NULL)
      return;

   CSourceItem* file = ProjectSearcher::findItemByUuid<CSourceItem>(m_pProject, uuid);
   if (file == NULL)
      return;

   file->setName(path);
   file->setPath(path);
   emit sourceFileChanged(uuid);
}
