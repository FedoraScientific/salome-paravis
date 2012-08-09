// Copyright (C) 2010-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "TableParser.h"

#include <QString>
#include <QStringList>

// STL includes
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

int getLine(std::ifstream& streamIn, QString& str)
{
  char tmp;
  std::ostringstream streamOut;
  
  while (streamIn.get(tmp)) {
    streamOut<<tmp;
    if (tmp == '\n') 
      break;
  }
  
  streamOut<<std::ends;
  str = streamOut.str().c_str();
  
  return !streamIn.eof();
}


bool Table2D::Check()
{
  if (myRows.empty()) 
    return false;

  int iEnd = myRows[0].myValues.size();
  if (iEnd == 0)
    {
      return false;
    }

  if (myColumnTitles.size() != iEnd) 
    {
      myColumnTitles.resize(iEnd);
    }

  if (myColumnUnits.size() != iEnd)
    {
      myColumnUnits.resize(iEnd);
    }

  int jEnd = myRows.size();
  for (int j = 0; j < jEnd; j++)
    {
      if (myRows[j].myValues.size() != iEnd)
	{
	  return false;
	}
    }
  
  return true;
}

std::vector<std::string> GetTableNames(const char* fname, const char* separator,
				       const bool firstStringAsTitles)
{
  Table2D table;
  std::vector<std::string> tableTitles;

  int nb = 0;
  table = GetTable(fname, separator, nb, firstStringAsTitles);
  while (table.Check()) {
    tableTitles.push_back(table.myTitle);
    table = GetTable(fname, separator, ++nb, firstStringAsTitles);
  }
  
  return tableTitles;
}

Table2D GetTable(const char* fname, const char* separator, const int tableNb,
		 const bool firstStringAsTitles)
{
  std::ifstream streamIn(fname);
  
  if(!streamIn.good())
    {
      throw std::runtime_error("Unable to open input Post-Pro table file.");
    }
  
  QString tmp;
  int count = 0;
  do {
    // Find beginning of table (tables are separated by empty lines)
    while (getLine(streamIn, tmp) && tmp.trimmed() == "");

    Table2D table2D;

    bool isFirst = true;
    while (!streamIn.eof() && tmp.trimmed() != "") 
      {
	QString data = tmp.trimmed();
	QString cmt = "";
	QString keyword = "";

	// Split string to data and comment (comment starts from '#' symbol)
	int index = tmp.indexOf("#");
	if (index >= 0) 
	  {
	    data = tmp.left(index).trimmed();
	    cmt = tmp.mid(index+1).trimmed();
	  }

	// If comment is not empty, try to get keyword from it (separated by ':' symbol)
	if (!cmt.isEmpty()) 
	  {
	    int index1 = cmt.indexOf(":");

	    if (index1 >= 0) 
	      {
		QString tmpstr = cmt.left(index1).trimmed();
		if (tmpstr == QString("TITLE") ||
		    tmpstr == QString("COLUMN_TITLES") ||
		    tmpstr == QString("COLUMN_UNITS") ||
		    tmpstr == QString("COMMENT")) 
		  {
		    keyword = tmpstr;
		    cmt = cmt.mid(index1+1).trimmed();
		  }
	      }
	  }
	
	// If data is empty, process only comment
	if (data.isEmpty()) 
	  {
	    // If keyword is found, try to process it
	    // elsewise it is a simple comment, just ignore it
	    if (!keyword.isEmpty()) 
	      {
		if (keyword == QString( "TITLE" )) 
		  {
		    QString title = cmt;
		    if (table2D.myTitle != "") {
		      title = QString(table2D.myTitle.c_str()) + QString(" ") + title;
		    }
		    table2D.myTitle = title.toLatin1().constData();
		  }
		else if (keyword == QString("COLUMN_TITLES")) 
		  {
		    // Comment may contain column headers
		    QStringList strList = cmt.split("|", QString::SkipEmptyParts);

		    for ( int i = 0; i < strList.count(); i++ ) 
		      {
			QString tmpstr = strList[i].trimmed();
			table2D.myColumnTitles.push_back(tmpstr.toLatin1().constData());
		      }
		  }
		else if (keyword == QString("COLUMN_UNITS")) 
		  {
		    // Comment may contain column units
		    QStringList strList = cmt.split( " ", QString::SkipEmptyParts );
	    
		    for (int i = 0; i < strList.count(); i++) 
		      {
			QString tmpstr = strList[i].trimmed();
			table2D.myColumnUnits.push_back(tmpstr.toLatin1().constData());
		      }
		  }
		else if (keyword == QString("COMMENT")) 
		  {
		    // Keyword 'COMMENT' processing can be here,
		    // currently it is ignored
		  }
	      }
	    else {
	      // Simple comment processing can be here,
	      // currently it is ignored
	    }
	  }
	// If data is not empty, try to process it
	else {
	  Table2D::Row row;
	
	  QString datar1 = data.replace(QRegExp("\t"), " ");
	  QStringList valList = datar1.split(separator, QString::SkipEmptyParts);
	  if(table2D.myColumnTitles.size() == 0 && isFirst && firstStringAsTitles) 
	    {
	      for ( int i = 0; i < valList.count(); i++ ) 
		{
		  QString tmpstr = valList[i].trimmed();
		  table2D.myColumnTitles.push_back(tmpstr.toLatin1().constData());
		}
	    }
	  else 
	    {
	      if (!cmt.isEmpty()) 
		{
		  row.myTitle = cmt.toLatin1().constData();
		}
          
	      for (int i = 0; i < valList.count(); i++) 
		{
		  if (valList[i].trimmed() != "") 
		    {
		      Table2D::Value val = valList[i].trimmed().toLatin1().constData();
		      row.myValues.push_back(val);
		    }
		}
	  
	      if(row.myValues.size() > 0)
		{
		  table2D.myRows.push_back(row);
		}
	    }

	  isFirst = false;
	}
	getLine(streamIn, tmp);
      }
    
    
    if(table2D.Check()) 
      {
	if (count == tableNb) 
	  {
	    if (QString::fromStdString(table2D.myTitle).isEmpty()) 
	      {
		table2D.myTitle = QString("Table:%1").arg(tableNb).toStdString();
	      }
	    return table2D;
	  }
	count++;
      }
    
  } while (!streamIn.eof());
  
  streamIn.close();
  
  // Return empty table
  Table2D emptyTable;
  return emptyTable;
}
