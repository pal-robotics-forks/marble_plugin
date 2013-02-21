#include "manage_kml_dialog.h"

#include "qt4/Qt/qcheckbox.h"
#include "qt4/Qt/qpushbutton.h"
#include "qt4/Qt/qfiledialog.h"
#include <iostream>

#include "ros/package.h"

#include <marble_plugin/ui_manage_kml_dialog.h>

namespace marble_plugin{

ManageKmlDialog::ManageKmlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manageKmlDialog)
{
    ui->setupUi(this);

    ui->kml_treeWidget->header()->show();
    ui->kml_treeWidget->header()->resizeSection(0, 630);
    ui->kml_treeWidget->header()->resizeSection(1, 45);


    std::map<QString, bool>::iterator it;
    for(it=m_kml_files.begin(); it != m_kml_files.end(); it++)
    {
        QString filepath = it->first;
        bool show = it->second;

        QFileInfo fi;
        fi.setFile(filepath);

        addKMLToTreeWiev(fi, show);
    }


    connect( ui->addButton , SIGNAL(clicked()) , this, SLOT( SetKMLFile() ));
    connect( ui->removeButton, SIGNAL(clicked()), this, SLOT(deleteKML()));
}

ManageKmlDialog::~ManageKmlDialog()
{
    delete ui;
}


void ManageKmlDialog::SetKMLFile( bool envoke_file_dialog )
{
    QFileInfo fi;

    if(envoke_file_dialog )
    {
        QString fn = QFileDialog::getOpenFileName( 0 ,
                                     tr("Open Geo Data File"), tr("") , tr("Geo Data Files (*.kml)"));
        fi.setFile( fn );
    }

    if( fi.isFile() )
    {
        bool show = true;
        addKMLToTreeWiev(fi, show);
        m_kml_files[fi.absoluteFilePath()] = show;
    }

}

void ManageKmlDialog::addKMLToTreeWiev(QFileInfo& kmlFile, bool show)
{
    QList<QTreeWidgetItem*> items = ui->kml_treeWidget->findItems( kmlFile.absoluteFilePath() , Qt::MatchExactly | Qt::MatchRecursive , 0 );
    if(items.size()>0)
    {
        return; // no doubles
    }

    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setText(0, kmlFile.absoluteFilePath() );
    item->setCheckState(1, Qt::Checked);
    ui->kml_treeWidget->addTopLevelItem( item );

}

void ManageKmlDialog::deleteKML()
{

    QList<QTreeWidgetItem*> items = ui->kml_treeWidget->selectedItems();

    if(items.size()==1)
    {
        QTreeWidgetItem* item = items.at(0);
        m_kml_files.erase(item->text(0));
        delete items.at(0);
    }
}

void ManageKmlDialog::checkBoxClicked(int state)
{

}

std::map< QString, bool> ManageKmlDialog::getKmlFiles()
{
    return m_kml_files;
}


}
