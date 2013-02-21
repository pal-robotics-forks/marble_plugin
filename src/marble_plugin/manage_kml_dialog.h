#ifndef MANAGE_KML_DIALOG_H
#define MANAGE_KML_DIALOG_H

#include <QDialog>
#include "qt4/Qt/qfileinfo.h"
#include <vector>
#include <map>

namespace Ui {
class manageKmlDialog;
}

namespace marble_plugin{


class ManageKmlDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ManageKmlDialog(std::map< QString, bool> &kml_files, QWidget *parent = 0);
    ~ManageKmlDialog();

    std::map< QString, bool> getKmlFiles();

    
    void addAllKMLsToTreeWiev(std::map< QString, bool>& kml_files);
private Q_SLOTS:
    void SetKMLFile(bool envoke_file_dialog = true );
    void deleteKML();
    void checkBoxClicked(int state);
    void cancelButtonClicked();
    void okButtonClicked();

private:
    void addKMLToTreeWiev(QFileInfo &kmlFile, bool show);

    Ui::manageKmlDialog *ui;
    std::map< QString, bool> m_kml_files;

};

}

#endif // MANAGE_KML_DIALOG_H
