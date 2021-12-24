#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <gdk/gdkkeysyms.h>
#include <json/json.h>
#include <pqxx/pqxx>

//#define WIN_NT

#ifdef WIN_NT
#include <windows.h>
#endif

#include "utils.h"
#include "ctasnif.h"
#include "gtester_ico.h"

using namespace std;
using namespace pqxx;

//consts

typedef struct
{
  gchar *tag;
  gchar *irbis_data;
  gchar *irbis_field_name;
}
Irbis_Item;

enum
{
  COLUMN_ITEM_TAG,
  COLUMN_ITEM_IRBIS_DATA,
  COLUMN_ITEM_IRBIS_FIELD_NAME,
  NUM_ITEM_COLUMNS
};

typedef struct
{
  gint     patentID;
  //gint     patentNumber;//pApplicationNum;
  gchar    *patentNumber;
  gchar    *pPatentForInvention;//*pPatentName;
  gchar    *pInventionTitle;//*pInventName;
  gchar    *pDateOfRequest;
  //gint     requestNumber;
  gchar    *requestNumber;
  gchar    *pPriorityDate;
  gchar    *pPatentHolder;//*pPatentOwnerName;
  gchar    *pAuthorsOfInvention;
  gchar    *pMaintenanceCondtion;
  gchar    *pAgencyForIntellectualProperty;
  gchar    *pDateOfTaxPayment;
}
Person_List_Item;
enum
{
  COLUMN_ITEM_PATENT_ID = 0,
  COLUMN_ITEM_PATENT_FOR_INVENTION,
  COLUMN_ITEM_PATENT_NUMBER,
  COLUMN_ITEM_INVENTION_TITLE,
  COLUMN_ITEM_DATE_OF_REQUEST,
  COLUMN_ITEM_REQUEST_NUMBER,
  COLUMN_ITEM_PRIORITY_DATE,
  COLUMN_ITEM_PATENT_HOLDER,
  COLUMN_ITEM_AUTHORS_OF_INVENTION,
  COLUMN_ITEM_MAINTENANCE_CONDTION,
  COLUMN_ITEM_AGENCY_FOR_INTELLECTUAL_PROP,
  COLUMN_ITEM_DATE_OF_TAX_PAYMENT,
  NUM_ITEMS_COLUMNS
};

/*enum
{
  COLUMN_ITEM_APPL_NUM=0,
  COLUMN_ITEM_PATENT_NAME,
  COLUMN_ITEM_INVENTION_NAME,
  COLUMN_ITEM_PATENT_OWNERNAME,
  COLUMN_ITEM_PATENT_NUM,
  COLUMN_ITEM_PATENT_APPLICATIOn_DELIVERED_DATE,
  NUM_ITEMS_COLUMNS
};*/


const int WORD_SIZE = 64;

const char ver[]   = { "0.9.9" };
const char prog_name[]= {"GArchiver"};

//GLOBALS

/*char* patent_fields_name [2][4]  = {
    { "firstName", "midName", "lastName", "userData" },
    { "firstName_2","midName_2","lastName_2", "userData_2"}
   };*/

typedef vector<string> table_fields_name;
vector<table_fields_name> patent_fields_name2;

bool is_cell_edited = false;
bool from_which_2_what = true; //keyboard layout cyr or lat
bool is_transliterate = false;


connection *conn2Postgres;

//CSystem *psys;
CTasnif *psys;
Irbis_Item *foo;

gboolean enable_all_controlls = TRUE;
bool is_insert_into_db = false;

gint patentID = -1;
int gWndWidth = 690, gWndHeight = 290;
int gl_vpaned_pos = 616, gl_hpaned_pos = 1040; //window settings
//int id = 0, id2 = 1, id3 = 2, id4 = 3;

char gTag_mark[5];
std::vector<std::string> pCmd;

fstream outf_data;
std::string alert_msg;
int num_messages = 0, gMarkDir = 0, g_mark_indx = 0;

CPathBuilder path_builder("/home/tkl/scanned");
CTransliter transliter;

Person_List_Item *person = nullptr;

static GArray *articles = NULL, *patent_list = NULL;
gchar *pCurrentDir = nullptr;

GtkApplication *gApp; //main application
GtkWidget *window = NULL; //main window
GtkWidget *view = NULL, *aux_view = NULL, *radio1 = NULL, *radio2;
GtkWidget *insert_into_db_btn = nullptr, *delete_button = nullptr;

//GtkWidget *treeview = NULL;
GtkTreeSelection *irbis_tree_selection, *query_patent_tree_selection;
GtkTreeModel *irbis_items_model;

GtkWidget *irbis_field_list = nullptr, *irbis_tree_view = nullptr;
GtkWidget *patent_tree_view;

GtkWidget *query_button, *quit_button, *save_button, *clear_button, *update_button;//*gImage;
//GtkTreeModel *items_model = NULL;
GtkListStore *tree_model = NULL, *patent_tree_model;
GtkCellRenderer   *arenderer[7];

GtkWidget *btn_start_getval, *but_corr_usb_accept;
GtkWidget *gl_name_entry, *gl_midname_entry, *gl_lastname_entry, *gl_birthdate_entry;
GtkEntryBuffer *name_entry_buffer, *midname_entry_buffer, *lastname_entry_buffer, *birthdate_entry_buffer;

PangoFontDescription *font_desc = NULL;
GdkPixbuf *myPixbuf = NULL;
static GdkRGBA /*text_color,*/ bg_color;

int build_sql_query_cmd(string &sql_str, int case_cmd = 0);

static void
add_irbis_items (void)
{
  //Irbis_Item foo;

  g_return_if_fail (articles != NULL);

  gint indx = 0;
  gchar *entry_text;
  vector< string >::const_iterator Iter;
  vector< string >  _seq = psys->get_irbis_name();//list of irbis fields name

  for(Iter = _seq.begin(); Iter != _seq.end(); Iter++ ,indx++)
  {
    //*Iter += '\n';
    //entry_text = (gchar*) Iter->c_str();
    entry_text =  g_convert( (gchar*) Iter->c_str(), -1, "UTF-8", "UTF-8", NULL, NULL, NULL);
    //indx = gtk_clist_append( GTK_CLIST(irbis_field_list) , &entry_text);
    foo->tag = g_strdup((gchar*)psys->get_marks(indx));
    foo->irbis_data = g_strdup("");
    foo->irbis_field_name = entry_text;
    g_array_append_vals (articles, foo, 1);
    //cout << Iter->c_str() << endl;
  }

}


static void
add_patent_tree_columns (GtkTreeView  *treeview/*,
             GtkTreeModel *items_model,
             GtkTreeModel *numbers_model*/)
{
  gint col_offset, mcase;
  
  GtkCellRenderer   *renderer;
  GtkTreeViewColumn *column;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  vector< string >  _seq = psys->get_irbis_name();//list of irbis fields name

  /* patent ID */

  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_ID));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            "ID", //"authors of invention",
                                                            renderer,
                                                            "text", COLUMN_ITEM_PATENT_ID,
                                                            //"visible", VISIBLE_COLUMN,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_ID));

 /* Prop yummy column */
  arenderer[0] = gtk_cell_renderer_text_new();
  /*renderer = gtk_cell_renderer_combo_new();

  g_object_set (renderer,
                "model", numbers_model,
                "text-column", COLUMN_PROP_COMBO,
                "has-entry", FALSE,
                "editable", TRUE,
                NULL);


  g_signal_connect (renderer, "edited", G_CALLBACK (cell_edited), items_model);*/
  //g_signal_connect (arenderer[0], "edited",  G_CALLBACK (cell_edited), items_model);

  g_object_set_data (G_OBJECT (arenderer[0]), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_NUMBER));
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1, 
                                               &(_seq[1])[0], //patent raqami 
                                               arenderer[0],
                                               "text", COLUMN_ITEM_PATENT_NUMBER,
                                               //"visible", VISIBLE_COLUMN,
                                               NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_PATENT_NUMBER);
  g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_NUMBER));


  arenderer[6] = gtk_cell_renderer_text_new ();

  //g_object_set (renderer,  "xalign", 0.0, NULL);
  //g_object_set_data (G_OBJECT (arenderer[4]), "column", GINT_TO_POINTER (COLUMN_ITEM_PRODUCT));

  //g_signal_connect (arenderer[4], "edited",  G_CALLBACK (cell_edited), items_model);

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[4])[0], //talabnoma raqami
                                                            arenderer[6], "text",
                                                            COLUMN_ITEM_REQUEST_NUMBER,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_REQUEST_NUMBER));
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_REQUEST_NUMBER);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);


  arenderer[4] = gtk_cell_renderer_text_new ();

  //g_object_set (renderer,  "xalign", 0.0, NULL);
  //g_object_set_data (G_OBJECT (arenderer[4]), "column", GINT_TO_POINTER (COLUMN_ITEM_PRODUCT));

  //g_signal_connect (arenderer[4], "edited",  G_CALLBACK (cell_edited), items_model);
  
  
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[0])[0], //ixtiroga patent
                                                            arenderer[4], "text",
                                                            COLUMN_ITEM_PATENT_FOR_INVENTION,
                                                            NULL);
  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_FOR_INVENTION));
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_PATENT_FOR_INVENTION);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
  gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);


  /* number column */
  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_INVENTION_TITLE));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[2])[0], //"Invention title",
                                                            renderer,
                                                            "text", COLUMN_ITEM_INVENTION_TITLE,
                                                            //"visible", VISIBLE_COLUMN,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_INVENTION_TITLE);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_INVENTION_TITLE));
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
    gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);

    /*factProdID column*/
    arenderer[5] = gtk_cell_renderer_text_new();
    //g_signal_connect (arenderer[5], "edited",  G_CALLBACK (cell_edited), items_model);
    g_object_set_data (G_OBJECT (arenderer[5]), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_HOLDER));
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1,
                                               &(_seq[6])[0], //"patent holder",
                                               arenderer[5],
                                               "text", COLUMN_ITEM_PATENT_HOLDER,
                                               //"visible", VISIBLE_COLUMN,
                                               NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_PATENT_HOLDER);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_PATENT_HOLDER));
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
    gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);

   /*col Quant*/
  arenderer[1] = gtk_cell_renderer_text_new();
  //g_signal_connect (arenderer[1], "edited",  G_CALLBACK (cell_edited), items_model);
  g_object_set_data (G_OBJECT (arenderer[1]), "column", GINT_TO_POINTER (COLUMN_ITEM_DATE_OF_REQUEST));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1,
                                               &(_seq[3])[0], //"Date of request",
                                               arenderer[1],
                                               "text", COLUMN_ITEM_DATE_OF_REQUEST,
                                               //"visible", VISIBLE_COLUMN,
                                               NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_DATE_OF_REQUEST);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_DATE_OF_REQUEST));
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
    gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);


  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_PRIORITY_DATE));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[5])[0], //"priority date",
                                                            renderer,
                                                            "text", COLUMN_ITEM_PRIORITY_DATE,
                                                            //"visible", VISIBLE_COLUMN,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_PRIORITY_DATE));
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
    gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);


  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_AUTHORS_OF_INVENTION));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[7])[0], //"authors of invention",
                                                            renderer,
                                                            "text", COLUMN_ITEM_AUTHORS_OF_INVENTION,
                                                            //"visible", VISIBLE_COLUMN,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_AUTHORS_OF_INVENTION);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_AUTHORS_OF_INVENTION));
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
    gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);


  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_MAINTENANCE_CONDTION));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[8])[0], //"authors of invention",
                                                            renderer,
                                                            "text", COLUMN_ITEM_MAINTENANCE_CONDTION,
                                                            //"visible", VISIBLE_COLUMN,
                                                            //"width", 256,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 256);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_MAINTENANCE_CONDTION));
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
    gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);


  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_AGENCY_FOR_INTELLECTUAL_PROP));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[9])[0], //"authors of invention",
                                                            renderer,
                                                            "text", COLUMN_ITEM_AGENCY_FOR_INTELLECTUAL_PROP,
                                                            //"visible", VISIBLE_COLUMN,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_AGENCY_FOR_INTELLECTUAL_PROP));


  renderer = gtk_cell_renderer_text_new();

  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",
                    G_CALLBACK (editing_started), NULL);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_DATE_OF_TAX_PAYMENT));

  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                                            -1, 
                                                            &(_seq[10])[0], //"authors of invention",
                                                            renderer,
                                                            "text", COLUMN_ITEM_DATE_OF_TAX_PAYMENT,
                                                            //"visible", VISIBLE_COLUMN,
                                                            NULL);

    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_ITEM_DATE_OF_TAX_PAYMENT);
    g_object_set_data (G_OBJECT (column), "column", GINT_TO_POINTER (COLUMN_ITEM_DATE_OF_TAX_PAYMENT));
}

static GtkListStore *
create_patent_tree_model (void)
{
  gint i = 0;
  GtkListStore *model;
  GtkTreeIter iter;

  /* create array */
  //patent_list = g_array_sized_new (FALSE, FALSE, sizeof (Person_List_Item), 1);

  //add_irbis_items ();

  /* create list store */
  model = gtk_list_store_new (NUM_ITEMS_COLUMNS,
                              G_TYPE_INT, //patentID
                              G_TYPE_STRING, //patent's name
                              G_TYPE_STRING, //patent's number
                              G_TYPE_STRING, //invention's naame
                              G_TYPE_STRING, //owner name
                              G_TYPE_STRING, //request Num
                              G_TYPE_STRING, //
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING);//patent request date

  return GTK_LIST_STORE (model);
}


static GtkTreeModel *
create_items_model (void)
{
  gint i = 0;
  GtkListStore *model;
  GtkTreeIter iter;

  /* create array */
  articles = g_array_sized_new (FALSE, FALSE, sizeof (Irbis_Item), 1);

  add_irbis_items ();

  /* create list store */
  model = gtk_list_store_new (NUM_ITEM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING,
                              G_TYPE_STRING);

  /* add items */
  for (i = 0; i < articles->len; i++)
    {
      gtk_list_store_append (model, &iter);

      gtk_list_store_set (model, &iter,
                          COLUMN_ITEM_TAG,
                          g_array_index (articles, Irbis_Item, i).tag,
                          COLUMN_ITEM_IRBIS_FIELD_NAME,
                          g_array_index (articles, Irbis_Item, i).irbis_field_name,
                          COLUMN_ITEM_IRBIS_DATA,
                          g_array_index (articles, Irbis_Item, i).irbis_data,
                          -1);
    }

  return GTK_TREE_MODEL (model);
}

static void
cell_edited (GtkCellRendererText *cell,
             const gchar         *path_string,
             const gchar         *new_text,
             gpointer             data)
{
  //cout << path_string << endl;
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
  GtkTreeIter iter;

  gint column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));

  gtk_tree_model_get_iter (model, &iter, path);
  //is_fixed = true;

  switch (column)
    {
    /*case COLUMN_ITEM_TAG:
      {
        gint i;

        i = gtk_tree_path_get_indices (path)[0];
        g_array_index (articles, Irbis_Item, i).tag = atoi (new_text);

        gtk_list_store_set (GTK_LIST_STORE (model), &iter, column,
                            g_array_index (articles, Irbis_Item, i).number, -1);
      }
      break;*/

    case COLUMN_ITEM_IRBIS_DATA:
      {
        //cout << "gotcha" << endl;
        gint i;
        gchar *old_text;
        
        //if (!is_cell_edited)
        {

           gtk_tree_model_get (model, &iter, column, &old_text, -1);
           g_free (old_text);
          

            i = gtk_tree_path_get_indices (path)[0];

            if (g_array_index (articles, Irbis_Item, i).irbis_data != nullptr)
               g_free (g_array_index (articles, Irbis_Item, i).irbis_data);
            
            if (strlen(new_text) > 2)
            {
              is_cell_edited = true;
            }
              g_array_index (articles, Irbis_Item, i).irbis_data = g_strdup (new_text);
             
               gtk_list_store_set (GTK_LIST_STORE (model), &iter, column,
                              g_array_index (articles, Irbis_Item, i).irbis_data, -1);
            
        }
      }
      break;
    }

  gtk_tree_path_free (path);
}


static void
add_columns (GtkTreeView  *treeview,
             GtkTreeModel *items_model
             /*GtkTreeModel *numbers_model*/)
{
  gint col_offset;

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

 /* yummy column */
  //renderer = gtk_cell_renderer_progress_new ();
  renderer = gtk_cell_renderer_text_new ();
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_TAG));

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1, "Tags", renderer,
                                               "text", COLUMN_ITEM_TAG,
                                               NULL);


  /* IRBIS field name column */
  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer,
                "wrap-width", 128,
                NULL);
  /*g_object_set (renderer,
                "wrap-mode",  PANGO_WRAP_CHAR,
                NULL);*/              
  g_object_set (renderer,
                "ellipsize", PANGO_ELLIPSIZE_END,
                NULL);
  /*g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);*/
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_IRBIS_FIELD_NAME));
  col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1, "Fields name", renderer,
                                               "text", COLUMN_ITEM_IRBIS_FIELD_NAME,
                                               //"width", 256,
                                               NULL);

  column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
  gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 256);
  gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
  gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN (column), TRUE);

  /* irbis data column */
  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer,
                "editable", TRUE,
                NULL);
  g_signal_connect (renderer, "edited",
                    G_CALLBACK (cell_edited), items_model);
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_IRBIS_DATA));

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
                                               -1, "Data", renderer,
                                               "text", COLUMN_ITEM_IRBIS_DATA,
                                               NULL);


}



static void
activate_quit (GSimpleAction *action, GVariant   *parameter, gpointer user_data);

static GActionEntry app_entries[] = {
  { "quit", activate_quit, NULL, NULL, NULL }
  //{ "dark", activate_toggle, NULL, "false", change_theme_state }
};

static void put_into_file(int num);
int InitDB();

/*call back functions */

static void
toggle_transliterate_cb (GtkToggleButton *check_button, gpointer data)
{
	if (gtk_toggle_button_get_active (check_button))
	{
		is_transliterate = true;
	}
	else
	{
		is_transliterate = false;
	}
}

static void
clear_IO_patent_tree_cb (GtkWidget *widget, GtkWidget *win)
{
  gint i = 0;
  GtkListStore *model;
  GtkTreeIter iter;

  g_return_if_fail(irbis_items_model != NULL);

  //gtk_list_store_clear(GTK_LIST_STORE(irbis_items_model));

  /* create list store */
  //model = gtk_list_store_new (NUM_ITEM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING,
  //                            G_TYPE_STRING);

  /* add items */
  /*for (i = 0; i < articles->len; i++)
    {
      gtk_list_store_append (model, &iter);

      gtk_list_store_set (model, &iter,
                          COLUMN_ITEM_TAG,
                          g_array_index (articles, Irbis_Item, i).tag,
                          COLUMN_ITEM_IRBIS_FIELD_NAME,
                          g_array_index (articles, Irbis_Item, i).irbis_field_name,
                          COLUMN_ITEM_IRBIS_DATA,
                          g_array_index (articles, Irbis_Item, i).irbis_data,
                          -1);
    }*/

  gtk_tree_model_get_iter_first(irbis_items_model, &iter);

  /* clear items */
  for (i = 0; i < articles->len; i++)
    {
      memset(g_array_index (articles, Irbis_Item, i).irbis_data, 0x0, sizeof(g_array_index (articles, Irbis_Item, i).irbis_data));
      //g_array_index (articles, Irbis_Item, i).irbis_data = "";
      //g_free(g_array_index (articles, Irbis_Item, i).irbis_data);
      //g_array_index (articles, Irbis_Item, i).irbis_data = g_strdup ("");
      //g_stpcpy(g_array_index (articles, Irbis_Item, i).irbis_data, "");

      /*g_snprintf (g_array_index (articles, Irbis_Item, i).irbis_data,
                  1,
                  "%c",
                   '');*/

      gtk_list_store_set (GTK_LIST_STORE( irbis_items_model), &iter,
                          COLUMN_ITEM_IRBIS_DATA,
                          g_array_index (articles, Irbis_Item, i).irbis_data,
                          -1);
      gtk_tree_model_iter_next(irbis_items_model, &iter);
    }
}

static void
delete_row_db_cb (GtkWidget *widget, GtkWidget *win)
{
  GtkWidget *dialog, *content_area, *label;

  gint response;

  char dTostr[20];

  string delete_sql_cmd("DELETE FROM inp_patents WHERE id = ");

  g_snprintf(dTostr, 10, "%d", patentID);
  delete_sql_cmd += dTostr;

  dialog = gtk_dialog_new_with_buttons("Confirmation",
                                   GTK_WINDOW (window),
                                   GTK_DIALOG_MODAL ,
                                   GTK_STOCK_YES,
                                   GTK_RESPONSE_YES,
                                   GTK_STOCK_NO,
                                   GTK_RESPONSE_NO,
                                   NULL);
  gtk_window_set_destroy_with_parent (GTK_WINDOW(dialog), TRUE);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  label = gtk_label_new ("<span color=\"#F90101\">Confirm that you are about to delete the row?</span>");
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_container_add (GTK_CONTAINER (content_area), label);

  //g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);

  gtk_widget_show_all(label);
  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if(response == GTK_RESPONSE_YES)
  {
    pqxx::work W{*conn2Postgres};
    //cout << delete_sql_cmd << endl;
    W.exec(delete_sql_cmd);
    W.commit();
  }
  //cout << delete_sql_cmd << endl;
  gtk_widget_destroy (dialog);
}

static void insert_into_db_cb (GtkWidget *wid, GtkWidget *win)
{
  string cmd;

  cmd  =  &(pCmd[0])[0]; //perl path
  cmd += " ";
  cmd += pCmd[1];

  /*cmd += pCurrentDir;
  cmd += "\\theAutoPutTags.txt ";
  cmd += pCurrentDir;
  cmd += "\\outPutFile.txt";*/

  is_insert_into_db = false;
  gtk_widget_set_sensitive(insert_into_db_btn, is_insert_into_db);
  //gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (auto_inc_check), is_insert_into_db);

  cout << &cmd[0] << endl;
  system( (char*)&cmd[0]);
}

static void connect2DB (GtkWidget *wid, gpointer data)
{
  GtkWidget *dialog = NULL;
  GtkWidget *button = GTK_WIDGET(data);

  if(num_messages > 0)
  {
    if ( InitDB() == 0 )
    {
      alert_msg.assign("Success! DataBase opened.");
      enable_all_controlls = TRUE;

      gtk_widget_set_sensitive(button, FALSE);

      gtk_widget_set_sensitive(query_button, enable_all_controlls);
      gtk_widget_set_sensitive(update_button, enable_all_controlls);
      gtk_widget_set_sensitive(delete_button, enable_all_controlls);
    }
  }

  dialog = gtk_message_dialog_new (GTK_WINDOW (window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, alert_msg.c_str());
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

template<typename V>
bool is_utf8_char(V val)
{
    auto ch = val;
    //int cc = ch;
    if ( isascii(ch) )
	return false;
    else
	return true;
}


static void
prep_sql_parameter(std::string &tmp, bool toCyrToLat = true)
{
	int indx = 1;
	gchar *result;
	std::string str;

	//result = g_utf8_strdown(static_cast<gchar*>(&tmp[0]), -1);

	/*if( result != NULL )
	{
		tmp.assign(static_cast<char*>(result));
		g_free(result);
	}*/

	if(!tmp.empty())
	{
		if(toCyrToLat)
		{
			str = std::string(1, tmp[0]);
			str +=std::string(1, tmp[1]);

			str = transliter.getCyr2Lat(str);
			/*std::for_each(str.begin(), str.end(), [](char & c) {
							c = ::toupper(c);
						});*/
			result = g_utf8_strup(static_cast<gchar*>(&str[0]), -1);

			if( result != NULL )
			{
				str.assign(static_cast<char*>(result));
				g_free(result);
			}

			tmp.assign(&tmp[2]);
			tmp.assign(str + transliter.getCyr2Lat(tmp));
		}
		//else
		{
			/*if (is_utf8_char(tmp[0]) )
			{
				str = std::string(1, tmp[0]);
				str +=std::string(1, tmp[1]);

				cout << str << endl;
				transliter.getLat2Cyr(str);
				//cout << str << endl;
				indx = 2;
			}
			else
			{
				str = std::string(1, tmp[0]);
				//cout << str << endl;
				transliter.getLat2Cyr(str);
				//cout << str << endl;
			}*/


			result = g_utf8_strup(static_cast<gchar*>(&str[0]), -1);
			str.assign(static_cast<char*>(result));
			g_free(result);

			//cout << str << endl;
			tmp.assign(&tmp[indx]);
			//str = "abcdŭëžŝ";
			//str = transliter.getLat2Cyr(str);

			//cout << tmp << endl;

      //is_transliterate ? str = str + transliter.getLat2Cyr(tmp) : str += tmp;

      if(is_transliterate) 
       str = str + transliter.getLat2Lat(tmp);
      else
        str += tmp;

			tmp.assign(str);
			//cout << "prep sql: " << tmp << endl;
		}

	}
}


int build_sql_query_cmd(string &sql_str, int case_cmd)
{

  const char x = '`';
  const char y = '\'';
  
  vector<string> db_fields_name;//(patent_fields_name[0][0], patent_fields_name[0]+3);
  
  for(int i=0; i < patent_fields_name2[0].size(); ++i)
  {
    db_fields_name.push_back(patent_fields_name2[0][i]);
  }
   
   std::size_t found;
   int connect_with_and = -1;
   int i = 0;

   //g_return_if_fail (articles != NULL);
   if (articles == NULL) 
    return -1;

   string corr_card, sql_query_str, tmp, cond_sql_str("inp_patent ->> ");
   
   switch(case_cmd)
   {
     case 0:
     {
       sql_query_str.assign("SELECT * FROM inp_patents WHERE ");
     }break;
     case 1:
     {
       if (is_cell_edited)
       {
         //sql_query_str.assign("UPDATE inp_patents SET inp_patent = jsonb_set(inp_patent::jsonb, ");
         sql_query_str.assign("UPDATE inp_patents SET inp_patent = inp_patent || '{");
       }        
     }break;
   }
   
  
   for (i = 0; i < articles->len; i++)
   {
      if( strlen((const char*)g_array_index (articles, Irbis_Item, i).irbis_data) != 0 )
      {
         ++connect_with_and;
         //cout << "articles: " << (const char*)g_array_index (articles, Irbis_Item, i).irbis_data << endl;
      }
    }
   
   //cout << "Num of ands " << connect_with_and << endl;
   if ( connect_with_and == -1 ) 
    return -1;

   for (i = 0; i < articles->len; i++)
   {
      if( strlen((const char*)g_array_index (articles, Irbis_Item, i).irbis_data) != 0 )
      {
         //corr_card += g_array_index (articles, Irbis_Item, i).tag;
         //corr_card += ' '; // ? why, unknown
         tmp = g_array_index (articles, Irbis_Item, i).irbis_data;

         //cout << "build sql: " << tmp << endl;

        std::replace(tmp.begin(), tmp.end(), y, x);
        //cout << tmp << endl;

        //if(is_transliterate)
        {
           /*if (from_which_2_what)
           {
              //cout << "UnTrans2\n";  
              tmp = UnTrans2(tmp, false);
           }
           else
           {
              //cout << "transliter.getCyr2Lat\n"; 
              tmp = transliter.getCyr2Lat(tmp);
           }*/
         }
         //corr_card += '\n';
         
         switch(case_cmd)
         {
           case 0:
           {
             sql_query_str += cond_sql_str + "'" + db_fields_name[i] + "' LIKE " + "'%" + tmp + "%'";
             if (connect_with_and != 0 )
             {
                sql_query_str += " AND ";
                --connect_with_and;
             }
           } break;
           case 1:
           {
             if (is_cell_edited)
             {
               sql_query_str += "\"" + db_fields_name[i] + "\": "; //+ "}'";
               if (connect_with_and != 0 )
               {
                  --connect_with_and;

                  found = db_fields_name[i].find("Number_depriciated"); //Number type changed to string
                  if (found != std::string::npos)
                  {
                    sql_query_str += tmp;
                  }
                  else
                  {
                    sql_query_str += "\"" + tmp + "\"";
                  }
                   
                  sql_query_str += ", ";
               }
               else
               {
                 sql_query_str += "\"" + tmp + "\"";
               }
             }             
           } break;
         }
      }
   }

   switch(case_cmd)
   {
     case 1:
     {
       sql_query_str += "}'";
     } break;
   }   
   //cout << "sql str:" << sql_query_str << endl;

   sql_str = sql_query_str;

   return connect_with_and;
}

void update_persons_tree_model (void)
{
  gint i = 0;
  GtkTreeIter iter;

  /* create array */
  if( patent_list == NULL )
  {
    //cout << "new array\n";
    patent_list = g_array_sized_new (FALSE, FALSE, sizeof (Person_List_Item), 1);
  }

  //add_irbis_items ();

  gtk_list_store_clear(GTK_LIST_STORE(patent_tree_model));
  //gtk_tree_store_clear(GTK_TREE_STORE(tree_model));

  //cout << "Update len:"<<person_list->len << endl;
  /* add items */
  for (i = 0; i < patent_list->len; i++)
    {
      //gtk_tree_store_append(GTK_LIST_STORE(tree_model), &iter, NULL);
      gtk_list_store_append (GTK_LIST_STORE(patent_tree_model), &iter);

      gtk_list_store_set (GTK_LIST_STORE(patent_tree_model), &iter,
                          COLUMN_ITEM_PATENT_ID,
                          g_array_index (patent_list, Person_List_Item, i).patentID,
                          COLUMN_ITEM_PATENT_FOR_INVENTION,
                          g_array_index (patent_list, Person_List_Item, i).pPatentForInvention,
                          COLUMN_ITEM_PATENT_NUMBER,
                          g_array_index (patent_list, Person_List_Item, i).patentNumber,
                          COLUMN_ITEM_INVENTION_TITLE,
                          g_array_index (patent_list, Person_List_Item, i).pInventionTitle,
                          COLUMN_ITEM_DATE_OF_REQUEST,
                          g_array_index (patent_list, Person_List_Item, i).pDateOfRequest,
                          COLUMN_ITEM_REQUEST_NUMBER,
                          g_array_index (patent_list, Person_List_Item, i).requestNumber,
                          COLUMN_ITEM_PRIORITY_DATE,
                          g_array_index (patent_list, Person_List_Item, i).pPriorityDate,
                          COLUMN_ITEM_PATENT_HOLDER,
                          g_array_index (patent_list, Person_List_Item, i).pPatentHolder,
                          COLUMN_ITEM_AUTHORS_OF_INVENTION,
                          g_array_index (patent_list, Person_List_Item, i).pAuthorsOfInvention,
                          COLUMN_ITEM_MAINTENANCE_CONDTION,
                          g_array_index (patent_list, Person_List_Item, i).pMaintenanceCondtion,
                          COLUMN_ITEM_AGENCY_FOR_INTELLECTUAL_PROP,
                          g_array_index (patent_list, Person_List_Item, i).pAgencyForIntellectualProperty,
                          COLUMN_ITEM_DATE_OF_TAX_PAYMENT,
                          g_array_index (patent_list, Person_List_Item, i).pDateOfTaxPayment,
                          -1);
    }

}

void fill_list_ckb_personnel(gchar *sql)
{
  string tmp_str;

  Json::Reader reader;
  Json::Value root;

  gint indx = 0;
  gchar *entry_text;

  //files_list_model = NULL;
  /* create array */
  g_array_free (patent_list, FALSE);
  patent_list = g_array_sized_new (FALSE, FALSE, sizeof (Person_List_Item), 1);

  g_return_if_fail ( patent_list != NULL);

  //cout << "check if fail" << endl;
  //gtk_tree_view_set_model (GTK_TREE_VIEW (file_tree_view), GTK_TREE_MODEL(files_list_model));

  //vector< string >::const_iterator Iter;
  //vector< string > _seq = psys->get_cards_file_name();//list of cards file name

      //sql = "select id, ckb_person->>'firstName', ckb_person->>'lastName', ckb_person->>'age' from ckb_persons as name WHERE \
		CAST ( ckb_person->>'age' AS INTEGER ) > 40";
      sql == nullptr ? sql = "SELECT * FROM ckb_persons where ckb_person->>'firstName' = 'Ibroxim'" : 0;
      //sql = "SELECT * FROM ckb_persons where ckb_person->>'firstName' = 'Ibroxim'";
      //sql = "SELECT * FROM ckb_persons WHERE CAST( ckb_person->>'birth_date' AS INTEGER  ) = 1955";
      nontransaction N(*conn2Postgres);
      result R( N.exec(sql) );

      //cout << "querying!\n";
      for(result::const_iterator c = R.begin(); c != R.end(); ++c, ++indx)
      {
        /*cout << indx << endl;
        if (indx == 3) break;*/
        //cout << c[0].as<int>() << endl;

        if(!reader.parse(c[1].as<string>(), root))
        {
            std::cout << reader.getFormattedErrorMessages();
            exit(1);
        }
        else
        {
            // Read and modify the json data
            //std::cout << "Size: " << root.size() << std::endl;
            //cout << root["firstName"] <<endl;

            //std::cout << "Contains nums? " << root.isMember("nums") << std::endl;
            //root["middle"] = "Danger";
        }

        //c[0].as<int>() << endl;
        person->patentID = c[0].as<int>();
        //prep_sql_parameter(tmp_str, false);
        //cout << "Patent ID: " << person->patentID << endl;

        //tmp_str = root["firstName"].as<string>();
        tmp_str = root[patent_fields_name2[0][0]].as<string>();

        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        
        //cout << "gotcha: " << tmp_str << " " << patent_fields_name2[0][0] << endl;
        //prep_sql_parameter(tmp_str, false);
        //cout << "gotcha: " << tmp_str << endl;

        person->pPatentForInvention = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        //tmp_str = root["lastName"].as<string>();
        tmp_str = root[patent_fields_name2[0][2]].as<string>();

        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        //prep_sql_parameter(tmp_str, false);
        //cout << "LastName: " << tmp_str << " " << patent_fields_name2[0][2] << endl;
        person->pInventionTitle = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        //cout << person->pName << endl;

        //tmp_str = root["midName"].as<string>();
        //person->patentNumber = root[patent_fields_name2[0][1]].as<int>();
        tmp_str = root[patent_fields_name2[0][1]].as<string>();
        
        tmp_str == " " ? tmp_str = "\0" : tmp_str;

        person->patentNumber = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);
        //cout << "Patent Num check: " << tmp_str << " " << root[patent_fields_name2[0][1]] << endl;

        //person->requestNumber = root[patent_fields_name2[0][4]].as<int>();
        tmp_str = root[patent_fields_name2[0][4]].as<string>();
        
        tmp_str == " " ? tmp_str = "\0" : tmp_str;

        person->requestNumber = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        tmp_str = root[patent_fields_name2[0][3]].as<string>();

        tmp_str == " " ? tmp_str = "\0" : tmp_str;

        //cout << "3: " << tmp_str << " " << patent_fields_name2[0][3] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pDateOfRequest = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);


        tmp_str = root[patent_fields_name2[0][5]].as<string>();

        tmp_str == " " ? tmp_str = "\0" : tmp_str;

        //cout << "5: " << tmp_str << " " << patent_fields_name2[0][5] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pPriorityDate = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        tmp_str = root[patent_fields_name2[0][6]].as<string>();
        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        //cout << "6: " << tmp_str << " " << patent_fields_name2[0][6] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pPatentHolder = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        tmp_str = root[patent_fields_name2[0][7]].as<string>();
        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        //cout << "7: " << tmp_str << " " << patent_fields_name2[0][7] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pAuthorsOfInvention = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        tmp_str = root[patent_fields_name2[0][8]].as<string>();
        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        //cout << "8: " << tmp_str << " " << patent_fields_name2[0][8] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pMaintenanceCondtion = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        tmp_str = root[patent_fields_name2[0][9]].as<string>();
        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        //cout << "9: " << tmp_str << " " << patent_fields_name2[0][9] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pAgencyForIntellectualProperty = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);

        tmp_str = root[patent_fields_name2[0][10]].as<string>();
        tmp_str == " " ? tmp_str = "\0" : tmp_str;
        //cout << "10: " << tmp_str << " " << patent_fields_name2[0][10] << endl;
        //prep_sql_parameter(tmp_str, false);
        person->pDateOfTaxPayment = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);        
        
        //cout << "indx" << indx << endl;
	    /*if(tmp_str.compare("Unknown") == true)
	    {
			cout << "Midname check: " << tmp_str << endl;
			person->pMidName = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);
		}
		else*/
		{
			//cout << "Midname check" << tmp_str << endl;
        //prep_sql_parameter(tmp_str, false);
        //person->patentNumber = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL) : tmp_str;
		}

	    /*tmp_str = c[2].as<string>();
	    person->pMidName = g_convert( (gchar*)&tmp_str[0], -1, "UTF-8", "UTF-8", NULL, NULL, NULL);*/

         //person->birth_date = root["birth_date"].as<int>();
         //person->birth_date > 0  ? person->birth_date : person->birth_date = -1;

         g_array_append_vals (patent_list, person, 1);
      }

  //update_persons_tree_model();
  //gtk_widget_grab_focus( GTK_WIDGET( file_tree_view ) );

}

static void
do_update(GtkWidget *wid, gpointer pID)
{
  GtkWidget *dialog, *content_area, *label;

  gint response;

  char dTostr[20];

  string update_sql_cmd;

  if ( build_sql_query_cmd(update_sql_cmd, 1) != -1 )
   {
      g_snprintf(dTostr, 10, "%d", patentID);
      update_sql_cmd += " WHERE id = ";
      update_sql_cmd +=  dTostr;
     //update_persons_tree_model();
   }

  

  dialog = gtk_dialog_new_with_buttons("Confirmation",
                                   GTK_WINDOW (window),
                                   GTK_DIALOG_MODAL ,
                                   GTK_STOCK_YES,
                                   GTK_RESPONSE_YES,
                                   GTK_STOCK_NO,
                                   GTK_RESPONSE_NO,
                                   NULL);
  gtk_window_set_destroy_with_parent (GTK_WINDOW(dialog), TRUE);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  label = gtk_label_new ("<span color=\"#F90101\">Confirm that you are about to update the row?</span>");
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_container_add (GTK_CONTAINER (content_area), label);

  //g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);

  gtk_widget_show_all(label);
  response = gtk_dialog_run (GTK_DIALOG (dialog));
  if(response == GTK_RESPONSE_YES)
  {
    pqxx::work W{*conn2Postgres};
    cout << update_sql_cmd << endl;
    W.exec(update_sql_cmd);
    W.commit();
  }
  gtk_widget_destroy (dialog);
}

static void do_query(GtkWidget *wid, gpointer pID)
{
  bool shallWeGo = false;
  int b = 0, *pId;
  int bits[] = {0,0,0};
  string tmp, sql;

  /*if(pID != nullptr)
  {
    //pId = reinterpret_cast<int*>(id);
    pId = reinterpret_cast<int*>(pID);
    switch(*pId)
    {
      case 0:
	{
	  tmp = static_cast<string>(gtk_entry_get_text (GTK_ENTRY (gl_name_entry)));
          //cout << tmp << endl;
	  (*pId) = 1;
          if(!gtk_widget_has_focus (GTK_WIDGET(gl_midname_entry)))
	   gtk_widget_grab_focus (GTK_WIDGET(gl_midname_entry));
	}break;
      case 1:
	{
          tmp = static_cast<string>(gtk_entry_get_text (GTK_ENTRY (gl_midname_entry)));
          //cout << tmp << endl; 
	  (*pId) = 2;
          //gtk_widget_activate (GTK_WIDGET(gl_name_entry));
          //if(gtk_widget_is_focus (GTK_WIDGET(gl_midname_entry)))
	  if(!gtk_widget_has_focus (GTK_WIDGET(gl_lastname_entry)))
            gtk_widget_grab_focus (GTK_WIDGET(gl_lastname_entry));
        }break;
      case 2:
	{
          tmp = static_cast<string>(gtk_entry_get_text (GTK_ENTRY (gl_lastname_entry)));
          //cout << tmp << endl; 
	  (*pId) = 0;
          if(!gtk_widget_has_focus (GTK_WIDGET(gl_name_entry)))
            gtk_widget_grab_focus (GTK_WIDGET(gl_name_entry));
	}break;
    }
    cout << *pId << endl;
  }*/
  //tmp = static_cast<string>(gtk_entry_get_text (GTK_ENTRY (gl_lastname_entry)));
   
   if ( build_sql_query_cmd(sql) != -1 )
   {
     fill_list_ckb_personnel((gchar*)&sql[0]);
     update_persons_tree_model();
   }
    
}


int let_2_focus = 1;
string tmp, initials_of_file_name, name_of_scanned_file;
string str;
//std::string out2fl, out2_tmp;

static void put_into_file(int num)
{
	gchar *result;
	std::string out2fl, out2_tmp;
	
	out2fl.assign(str);
	//out2fl = transliter.getCyr2Lat(out2fl);
	/*std::for_each(out2fl.begin(), out2fl.end(), [](char & c) {
			c = ::toupper(c);
			});*/
	result = g_utf8_strup(static_cast<gchar*>(&out2fl[0]), -1);
	if( result != NULL )
	{
	    out2fl.assign(static_cast<char*>(result));
	    g_free(result);
	}
	out2_tmp.assign(&tmp[2]);
	//out2_tmp = transliter.getCyr2Lat(out2_tmp);
	out2fl.append(out2_tmp);
	outf_data << "#" << num << " " << out2fl << endl;	
}


/*the end of the call back functions */

void GetNextMark( int direct = 1)
{
    //gMarkDir = dir;
    int count = psys->get_marks_num();

    //cout << "getnextmark: " << g_mark_indx << endl;
  /*if (direct == 0)
  {
    switch(gMarkDir)
     {
       case 0: ++g_mark_indx; break;
       case 1: --g_mark_indx;  break;
     }

     if ( g_mark_indx >=  count-1 )
     {
        //g_mark_indx = 0;
       g_mark_indx = count-1;
       gMarkDir = 1;
     }
     if (g_mark_indx <= 0 )
     {
       //g_mark_indx = count-1;
       g_mark_indx = 0;
       gMarkDir = 0;
     }
  }*/ 

     //cout << gMarkDir << " " <<g_mark_indx << endl;
     //strcpy(gTag_mark, psys->get_marks(g_mark_indx) );


  if (g_mark_indx <= count )
  {
    //gint x;
    GtkTreeViewColumn *column = nullptr;
    GtkTreePath *path = nullptr;
    //gchar *path_str = nullptr;

    //gtk_tree_view_get_cursor(GTK_TREE_VIEW(irbis_tree_view), &path, &column);
    //column = gtk_tree_view_get_column(GTK_TREE_VIEW(irbis_tree_view), 2);
    
    //if(column != NULL) x = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (column), "column"));

    /*path_str = gtk_tree_path_to_string (path);
    cout << "getnextmark2: " << path_str << " x: " << x << endl;
    g_free(path_str);*/

    //cout << "getnextmark2: " << g_mark_indx << endl;
    path = gtk_tree_path_new_from_indices (g_mark_indx, -1);
    
    //gtk_widget_grab_focus (irbis_tree_view);
    //gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(irbis_tree_view), path, column, NULL, FALSE);

    /*path_str = gtk_tree_path_to_string (path);
    cout << "getnextmark2: " << path_str << endl;
    g_free(path_str);*/
     //cout << "getnextmark2: " << g_mark_indx << endl;
     
     /*gtk_tree_view_row_activated (GTK_TREE_VIEW (irbis_tree_view),
                                                         path,
                                                         column
                                                         );*/

     gtk_tree_view_set_cursor (GTK_TREE_VIEW (irbis_tree_view),
                                                         path,
                                                         NULL, //column,
                                                         FALSE);
    gtk_tree_path_free (path);
     
   }
}

static bool getKeyboardLayout()
{
	bool ret = false;
  unsigned short layout = 1033;
#ifdef WIN_NT
	unsigned short layout = LOWORD(GetKeyboardLayout(0)); //current keyboard layout
#endif
	
	switch(layout)
	{
		case 1049:
		{
			//cyrillic, return false
		}break;
		case 2057:
		{
			//uk
		};
		case 1033: ret = true; break; //us
	}
	
	return ret;
}

static gboolean
key_press_cb (GtkWidget *text_view,
                 GdkEventKey *event)
{
  GtkTreeSelection * sel;
  GtkTreeIter iter, parent_iter;
  GtkTextBuffer *buffer;
  GtkTreePath *path;
  GtkTreeViewColumn *column;
  gint n=0, i, x, prodID;


    //gtk_tree_view_get_cursor(GTK_TREE_VIEW(text_view), &path, &column);
    //if(column != NULL) x = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (column), "column"));

    //g_print ("You selected a tag %s\n", gTag_mark);
   if (gtk_widget_is_focus(irbis_tree_view))
   {
     //gtk_tree_view_get_cursor(GTK_TREE_VIEW(irbis_tree_view), &path, &column);
    if (g_strcmp0(gTag_mark, "#") != 0)
    {
        from_which_2_what = getKeyboardLayout();
		   if ((event->state & GDK_CONTROL_MASK) != GDK_CONTROL_MASK ||
           (event->state & GDK_MOD1_MASK) != GDK_MOD1_MASK )
          {
            if ( (event->keyval >= GDK_KEY_0 ) && (event->keyval <= GDK_KEY_9) || 
                (event->keyval >= GDK_KEY_A) && (event->keyval <= GDK_KEY_Z ) ||
                (event->keyval >= GDK_KEY_a) && (event->keyval <= GDK_KEY_z ) 
               )
            {
              gtk_tree_view_get_cursor(GTK_TREE_VIEW(irbis_tree_view), &path, &column);
              gtk_tree_view_set_cursor(GTK_TREE_VIEW(irbis_tree_view), path, column, TRUE);
              gdk_event_put( (GdkEvent*)event );
              return TRUE;
            }
          }
          if(event->keyval == GDK_KEY_Down)
            {
              ++g_mark_indx;

              //gtk_tree_model_get_iter(irbis_items_model, &iter, path);
              //sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(text_view));
              //if(gtk_tree_selection_iter_is_selected (sel, &iter))
              {
                  //gtk_tree_selection_unselect_iter (sel, &iter);
                  //gtk_tree_selection_unselect_all(sel);
              }

              GetNextMark();
              //gdk_event_put( (GdkEvent*)event );
              return TRUE;
            }
            if (event->keyval == GDK_KEY_Up)
            {
              g_mark_indx != 0 ? --g_mark_indx : g_mark_indx;
              GetNextMark();
              return TRUE;
            }
    }
    else
    {
      if(event->keyval == GDK_KEY_Down)
      {
        //gdk_event_put( (GdkEvent*)event );
        g_mark_indx ^= g_mark_indx;
        //cout << "Down: " << g_mark_indx << endl;
        GetNextMark();
        return TRUE;
      }
    }
  }  
  if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
  {
    if ((event->keyval == GDK_KEY_L) || (event->keyval == GDK_KEY_l))
    {
      if (!gtk_widget_is_focus(irbis_tree_view))
      { 
             gtk_tree_selection_unselect_all (query_patent_tree_selection);
            
             gtk_widget_grab_focus(irbis_tree_view);
             gtk_tree_view_get_cursor(GTK_TREE_VIEW(irbis_tree_view), &path, &column);
      }
    }
  }
    if(event->keyval == GDK_KEY_Return && x < 2)
    {
        string str;

        //gtk_tree_view_get_cursor(GTK_TREE_VIEW(text_view), &path, &column);
        //cout << gtk_tree_view_column_get_x_offset(column) << endl;
        //column = gtk_tree_view_get_column(GTK_TREE_VIEW(text_view), 3);
        //if(column != NULL) x = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (column), "column"));
        //gtk_tree_view_column_cell_get_position(column, arenderer[0], &x, NULL);

        //gtk_tree_model_get_iter(items_model, &iter, path);
        

        //cout << str << i << " " << map_prods[str][i].miqdor << endl;

    }

    switch (event->keyval)
    {
      case GDK_KEY_F2:
      {
        g_signal_emit_by_name(G_OBJECT(save_button), "clicked");
      }
      break;
      case GDK_KEY_F3:
      {
        if (enable_all_controlls)
        {
          g_signal_emit_by_name(G_OBJECT(query_button), "clicked");
        }
      }
      break;
      case GDK_KEY_F5:
      {
        if (enable_all_controlls)
        {
          g_signal_emit_by_name(G_OBJECT(update_button), "clicked");
        }
      }
      break;
      case GDK_KEY_F7:
      {
        g_signal_emit_by_name(G_OBJECT(insert_into_db_btn), "clicked");
      }
      break;
      case GDK_KEY_F8:
      {
        if (enable_all_controlls)
        {
           g_signal_emit_by_name(G_OBJECT(delete_button), "clicked");
        }
      }
      break;
      case GDK_KEY_F12:
      {
        g_signal_emit_by_name(G_OBJECT(quit_button), "clicked");
      }
      break;
      case GDK_KEY_F9://GDK_KEY_Escape :
      {
        g_signal_emit_by_name(G_OBJECT(clear_button), "clicked");
      }
      break;

    }

  return FALSE;
}


static void
tree_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
        GtkTreeIter iter;
        GtkTreeModel *model;
        GtkTreePath  *path;
        GtkTreeViewColumn *focus_col;
        gchar *tag;
        
        gtk_tree_view_get_cursor(GTK_TREE_VIEW(irbis_tree_view), &path, NULL);
        gtk_tree_model_get_iter (irbis_items_model, &iter, path);

        focus_col = gtk_tree_view_get_column(GTK_TREE_VIEW(irbis_tree_view), 2);

        if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
                gtk_tree_model_get (model, &iter, COLUMN_ITEM_TAG, &tag, -1);
                gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(irbis_tree_view), path, focus_col, NULL, FALSE);

                strcpy(gTag_mark, tag);
                //g_print ("You selected a tag %s\n", gTag_mark);

                //is_mark_changed = true;
                //gtk_entry_set_text(GTK_ENTRY(mark_view), (const gchar *)m_mark);
                g_free (tag);
        }
}

static void
query_patent_list_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
        GtkTreeIter iter, irbis_iter;
        GtkTreeModel *model; //= GTK_TREE_MODEL(data);
        GtkTreePath  *path;
        GtkTreeViewColumn *focus_col;
        gchar *patent_data;
        gchar dTostr[32];
        gint c = 0;

        gtk_tree_view_get_cursor(GTK_TREE_VIEW(patent_tree_view), &path, NULL);
        gtk_tree_model_get_iter (GTK_TREE_MODEL(patent_tree_model), &iter, path);

        //focus_col = gtk_tree_view_get_column(GTK_TREE_VIEW(patent_tree_view), 1);

        if (gtk_tree_selection_get_selected (selection, &model, &iter))
        {
          gtk_tree_model_get_iter_first(irbis_items_model, &irbis_iter);
                for (int i = 0, c = 0; i < articles->len-1; ++c)
                {
                  switch(c)
                  {
                    case 0:
                    {
                      //--i;
                      //gtk_tree_model_iter_next(irbis_items_model, &irbis_iter);
                      continue; //cause it's int type
                    } break;
                    case 13://2:
                    {
                      //gtk_tree_model_iter_next(irbis_items_model, &irbis_iter);
                      //continue; //cause it's int type
                    } //break;
                    case 14://5:
                    {
                      gtk_tree_model_get (model, &iter, c, &patentID, -1);

                      //cout << i << " : " << patentID << endl;
                      
                      g_snprintf(dTostr, 10, "%d", patentID);
                      
                      if (g_array_index (articles, Irbis_Item, i).irbis_data != nullptr)
                         g_free(g_array_index (articles, Irbis_Item, i).irbis_data);

                      g_array_index (articles, Irbis_Item, i).irbis_data = g_strdup(dTostr);

                      //g_array_index (articles, Irbis_Item, i).irbis_data = patentID;

                      gtk_list_store_set (GTK_LIST_STORE (irbis_items_model),
                                     &irbis_iter,
                                     COLUMN_ITEM_IRBIS_DATA,
                                    g_array_index (articles, Irbis_Item, i).irbis_data,
                                     -1);

                      gtk_tree_model_iter_next(irbis_items_model, &irbis_iter);
                      
                      ++i;
                      continue; //cause it's int type
                    } break;
                  }
                  if (i < 11)
                  {
                     //get data from the bottom table's columns and fill top table's rows
                     gtk_tree_model_get (model, &iter, c/*patent columns names*/, &patent_data, -1);
                  
                     //cout << i << " : " << patent_data << endl;

                     if (g_array_index (articles, Irbis_Item, i).irbis_data != nullptr)
                        g_free(g_array_index (articles, Irbis_Item, i).irbis_data);
                     
                     if (patent_data != nullptr)
                     {
                       if(g_strcmp0(patent_data, "0") == 0)
                       {
                         strncpy(patent_data, "\0", 1);
                       }
                        g_array_index (articles, Irbis_Item, i).irbis_data = g_strdup(patent_data);

                        gtk_list_store_set (GTK_LIST_STORE (irbis_items_model),
                                         &irbis_iter,
                                         COLUMN_ITEM_IRBIS_DATA,
                                         g_array_index (articles, Irbis_Item, i).irbis_data,
                                        -1);

                        g_free(patent_data);
                     }
                     

                     gtk_tree_model_iter_next(irbis_items_model, &irbis_iter);
                     ++i;
                  }

                  //cout << g_array_index (articles, Irbis_Item, i).irbis_field_name << endl;
                }
                gtk_tree_model_get (model, &iter, COLUMN_ITEM_PATENT_ID, &patentID, -1);
                //gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(irbis_tree_view), path, focus_col, NULL, FALSE);

                //strcpy(gTag_mark, tag);
                
                //g_print ("You selected a id %d\n", patentID);

                //is_mark_changed = true;
                //gtk_entry_set_text(GTK_ENTRY(mark_view), (const gchar *)m_mark);
                //g_free (tag);
        }
}


void get_corrected_irbis_data()
{
   /*char* fields_name []  = {
     "firstName",
     "midName",
     "lastName"
   };*/
   //vector<string> db_fields_name(fields_name, fields_name+3);

   const char x = '`';
   const char y = '\'';

   int connect_with_and = -1;
   int i = 0;

   g_return_if_fail (articles != NULL);

   string corr_card, sql_query_str, tmp, cond_sql_str("ckb_person ->> ");

   //sql_query_str.assign("SELECT * FROM ckb_persons WHERE ");
  
   /*for (i = 0; i < articles->len; i++)
   {
      if( strlen((const char*)g_array_index (articles, Irbis_Item, i).irbis_data) != 0 )
      {
         ++connect_with_and;
      }
    }*/
   
   //cout << "Num of ands " << connect_with_and << endl;

   for (i = 0; i < articles->len; i++)
   {
      if( strlen((const char*)g_array_index (articles, Irbis_Item, i).irbis_data) != 0 )
      {
         corr_card += g_array_index (articles, Irbis_Item, i).tag;
         //corr_card += ' '; // ? why, unknown
         corr_card += tmp = g_array_index (articles, Irbis_Item, i).irbis_data;

         corr_card += '\n';

         //sql_query_str += cond_sql_str + "'" + db_fields_name[i] + "' LIKE '" + tmp + "%'";
         /*if (connect_with_and != 0 )
         {
           sql_query_str += " AND ";
           --connect_with_and;
         }*/
      }
      else if (g_strcmp0(g_array_index (articles, Irbis_Item, i).tag, "#") != 0)
      {
        corr_card += g_array_index (articles, Irbis_Item, i).tag;

        corr_card += " ";

        corr_card += '\n';
      }
   }
   corr_card += "*****\n";

   //cout << corr_card << endl;

   std::replace(corr_card.begin(), corr_card.end(), y, x);

   //if(is_transliterate)
   {

      /*if (from_which_2_what)
      {
        cout << "UnTrans2\n";  
        corr_card = UnTrans2(corr_card, false);
      }
      else
      {
         cout << "transliter.getCyr2Lat\n"; 
         corr_card = transliter.getCyr2Lat(corr_card);
      }*/
    }
   //cout << corr_card << endl;
   //cout << "sql str:" << sql_query_str << endl;
   psys->set_card_index(0);
   psys->set_corrected_irbis_card(corr_card);
}


static void save_in_file_event( GtkWidget      *widget,
                                    gpointer   data )
{
  gchar *tmpFilePath;
  string str;
  time_t rawTime;
	char buf_tm[20];
	//struct timeval timeNow;
	struct tm *_pTime;

   //cout << "just saving\n";
   if(is_cell_edited)
   {
       fstream ftmp("temp.txt", ios::out);
       ftmp.close();

       is_cell_edited  = false;
       
       get_corrected_irbis_data();
       psys->save_marked_cards(2); //data of irbis column written and saved in temp.txt file

       time( &rawTime );
       _pTime = localtime( &rawTime );

       strftime(buf_tm, sizeof(buf_tm),"%Y.%m.%d_%H%M%S", _pTime);

       tmpFilePath = g_get_current_dir();

       str.assign(buf_tm);
       str += ".txt";
       gchar *backup_path = g_build_filename(tmpFilePath, &str[0], NULL);

       g_rename("temp.txt", backup_path);
       outf_data << str << endl; //write down new file name into the list.txt

       g_free(tmpFilePath);
       g_free(backup_path);

       is_insert_into_db = true;
       gtk_widget_set_sensitive(insert_into_db_btn, is_insert_into_db);
   }
}

static void
activate_quit (GSimpleAction *action,
      GVariant   *parameter,
      gpointer    user_data)
{
 GApplication *app = G_APPLICATION(user_data);
 GtkWidget *dialog, *label, *content_area;
 //GtkApplication *app = GTK_APPLICATION(user_data);
 //GtkWidget *win;//GTK_WIDGET(user_data);
 //GList *list, *next;


 //list = gtk_application_get_windows ( GTK_APPLICATION(app) );

 /* while (list)
  {
    win = GTK_WIDGET(list->data);
    next = list->next;

    gtk_application_remove_window ( GTK_APPLICATION(app), GTK_WINDOW(win));
    //gtk_widget_destroy ( win );

    list = next;
  }*/
 gint response;
 gboolean pass_by = FALSE;
 if( parameter != NULL )
 {
     if(pass_by = g_variant_get_boolean(parameter))
        response = GTK_RESPONSE_YES;
 }
 dialog = gtk_dialog_new_with_buttons("Confirmation",
                                   GTK_WINDOW (window),
                                   GTK_DIALOG_MODAL ,
                                   GTK_STOCK_YES,
                                   GTK_RESPONSE_YES,
                                   GTK_STOCK_NO,
                                   GTK_RESPONSE_NO,
                                   NULL);
 gtk_window_set_destroy_with_parent (GTK_WINDOW(dialog), TRUE);

 content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
 label = gtk_label_new ("<span color=\"#F90101\">Do you really want to exit?!</span>");
 gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
 gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_CENTER);
 gtk_container_add (GTK_CONTAINER (content_area), label);

 //g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
 gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_NO);

 gtk_widget_show_all(label);
 pass_by == FALSE ? response = gtk_dialog_run (GTK_DIALOG (dialog)) : pass_by;
 if(response == GTK_RESPONSE_YES)
 {
    delete person;
    delete conn2Postgres;
    outf_data.close();

    gtk_widget_destroy(GTK_WIDGET(window));
    g_application_quit( G_APPLICATION(app) );
 }
 gtk_widget_destroy (dialog);
}

static void quit_app(GtkWidget *win, gpointer data)
{
 //system(gCmdLine);
 activate_quit(NULL, NULL, data);
}

void MessageBox( string &msg, GtkMessageType msgtype = GTK_MESSAGE_INFO)
{
    if(num_messages > 0)
    {
        GtkWidget *msgbox;
        //cout << "TKL: "<< msg << endl;

        msgbox = gtk_message_dialog_new(GTK_WINDOW (window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                                        GTK_MESSAGE_INFO,
                                                        GTK_BUTTONS_OK, &msg[0]);
        gtk_dialog_run(GTK_DIALOG(msgbox));
        gtk_widget_destroy(msgbox);

    }
}

void
combo_active_item_changed_cb (GtkComboBox *combo_box, gpointer  user_data)
{
  //GtkToolPalette *palette = GTK_TOOL_PALETTE (user_data);
  GtkTreeModel *model = gtk_combo_box_get_model (combo_box);
  GtkTreeIter iter;
  gint val = 1;

  if (!gtk_combo_box_get_active_iter (combo_box, &iter))
    return;

  gtk_tree_model_get (model, &iter, 1, &val, -1);

  if (val == -1)
    cout << val << endl;
  else
    //gtk_tool_palette_set_style (palette, val);
    cout << val << endl;
}

static gboolean
on_key_press(GtkWidget *widget, GdkEventKey *event)
{
  guint *keyval;
  //GdkKeymap keymap;
  //GdkMdodifierType consumed;

  if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
  {
    if ((event->keyval == GDK_KEY_L) || (event->keyval == GDK_KEY_l))
    {
      if (gtk_widget_is_focus(irbis_tree_view) != TRUE ) 
             gtk_widget_grab_focus(irbis_tree_view);
    }
  }
}

void 
about_us( GtkWidget *widget,
          gpointer   data )
{
 //GdkPixbuf *myPixbuf = gdk_pixbuf_from_pixdata(&GTesterLogo, FALSE, NULL);
 const gchar *authors[] = {
 "TKL K96 207",
   NULL
  };

// gtk_about_dialog_set_logo_icon_name()
 gtk_show_about_dialog (GTK_WINDOW(data),
        "program-name", prog_name,
        "copyright", "Muqobil Dasturlar To'plami (c) Build: July 21 2021 - GTK+ 3.6.4",
        "license-type", GTK_LICENSE_GPL_3_0,
        "version", ver,
        "comments", "O'zbekiston, Toshkent shahri\n  muqobildasturlar@gmail.com",
        "title", ("About GArchiver"),
        "authors", authors,
        //"logo-icon-name", "gtester.ico",
        "logo", (const GdkPixbuf*)myPixbuf,
       NULL);

 //g_free(myPixbuf);
}

GtkWidget * create_my_window(gpointer data)
{

 char buf_name[64];
 guint bus_watch_id, kbd_handler_id;

 GtkWidget *vpaned, *hpaned;
 //GtkApplicationWindow *window = NULL;
 GtkWidget *button = NULL, *transliterate_toggle = nullptr;//, *radio1, *radio2, *radio3, *entry;
 GtkWidget *local_entry1;
 //GtkWidget *win = NULL;
 GtkWidget *vbox = NULL, *hbox = NULL, *frame_vert = NULL;
 GtkWidget *sw = nullptr, *sw2 = nullptr;
 GtkWidget *table, *combo;
 GtkWidget *label = NULL, *overlay = NULL;
 GtkTextBuffer *buffer = NULL;

 GtkCellRenderer *cell_renderer = nullptr;
 GtkListStore *what_table_model = nullptr; 
 GtkTreeIter iter;
 //GThread *thread = NULL;
 gint is_DB_connected = -1;

  is_DB_connected = InitDB();
  if ( is_DB_connected < 0)
  {
     enable_all_controlls = FALSE;
     ++num_messages;//return NULL;
  } 

  strncpy(buf_name, prog_name, strlen((const char*)prog_name)+1);
  //strncat(buf_name, "-", 1);
  g_strlcat(buf_name, "-", sizeof(buf_name));
  g_strlcat(buf_name, ver, sizeof(buf_name));

 /* Create the main window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_container_set_border_width (GTK_CONTAINER (window), 3);

  gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);

  kbd_handler_id = g_signal_connect (G_OBJECT (window), 
                                    "key-press-event", 
                                    G_CALLBACK (key_press_cb),
                                    NULL);


  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);
  //gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 1);
  gtk_container_add (GTK_CONTAINER (window), hbox);


  //vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 3);
  //gtk_container_set_border_width (GTK_CONTAINER (vbox), 2);
  //gtk_container_add (GTK_CONTAINER (win), hbox);
  //gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 1);

  //frame_vert = gtk_frame_new (NULL);
  //gtk_widget_set_valign (frame_vert, GTK_ALIGN_FILL);
  //gtk_widget_set_size_request(frame_vert, 1, 71);
  //gtk_box_pack_start(GTK_BOX (hbox), frame_vert, TRUE, TRUE, 1);

  //overlay = gtk_overlay_new ();
  //gtk_container_add (GTK_CONTAINER (frame_vert), overlay);

  //vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  //gtk_container_add (GTK_CONTAINER (overlay), vbox);

  /* create models */
  irbis_items_model = create_items_model ();
  patent_tree_model = create_patent_tree_model ();

  /* create tree view */
  irbis_tree_view = gtk_tree_view_new_with_model (irbis_items_model);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (irbis_tree_view), TRUE);
  /*gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (irbis_tree_view)),
                                   GTK_SELECTION_SINGLE);*/
  gtk_tree_view_set_enable_search(GTK_TREE_VIEW (irbis_tree_view), false);

  add_columns (GTK_TREE_VIEW (irbis_tree_view), irbis_items_model);
  g_object_unref (irbis_items_model);

  //g_signal_connect(G_OBJECT(irbis_tree_view), "key-press-event", G_CALLBACK(key_press_cb), NULL);

  irbis_tree_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (irbis_tree_view));
  gtk_tree_selection_set_mode (irbis_tree_selection, GTK_SELECTION_SINGLE);
  g_signal_connect (G_OBJECT (irbis_tree_selection), "changed",
                  G_CALLBACK (tree_selection_changed_cb),
                  NULL);


sw = gtk_scrolled_window_new (NULL, NULL);
gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                           GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);

gtk_container_add (GTK_CONTAINER (sw), irbis_tree_view);

  /* create a vpaned widget and add it to our toplevel window */
  //hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);//gtk_hpaned_new ();
  
  //gtk_paned_add2(GTK_PANED(hpaned), vpaned);
  
  //gtk_container_add (GTK_CONTAINER (frame_vert), sw);


/* create patent select query tree list view */
  patent_tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL(patent_tree_model));
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (patent_tree_view), TRUE);

  /*gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (irbis_tree_view)),
                                   GTK_SELECTION_SINGLE);*/
  gtk_tree_view_set_enable_search(GTK_TREE_VIEW (patent_tree_view), false);

  add_patent_tree_columns (GTK_TREE_VIEW (patent_tree_view));//, patent_tree_model);
  g_object_unref (patent_tree_model);

  //g_signal_connect(patent_tree_view, "key-press-event", G_CALLBACK(key_press_cb), NULL);

  query_patent_tree_selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (patent_tree_view));
  gtk_tree_selection_set_mode (query_patent_tree_selection, GTK_SELECTION_SINGLE);
  g_signal_connect (G_OBJECT (query_patent_tree_selection), "changed",
                    G_CALLBACK (query_patent_list_selection_changed_cb),
                    NULL);
  
  

  sw2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw2),
                                         GTK_SHADOW_ETCHED_IN);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw2),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);

  gtk_container_add (GTK_CONTAINER (sw2), patent_tree_view);


  vpaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

  gtk_paned_set_wide_handle (GTK_PANED(vpaned), TRUE);
  
  gtk_paned_pack1 (GTK_PANED(vpaned), sw, TRUE, FALSE);
  gtk_paned_pack2 (GTK_PANED(vpaned), sw2, TRUE, FALSE);

  //gtk_paned_set_position( GTK_PANED (vpaned), gl_vpaned_pos );

  gtk_box_pack_start (GTK_BOX (hbox), vpaned, TRUE, TRUE, 1);
  //gtk_widget_show (vpaned);

  //gtk_paned_set_position( GTK_PANED (hpaned), gl_hpaned_pos );
  //gtk_widget_show (hpaned);

  //gl_vpaned_pos = gtk_paned_get_position( GTK_PANED (vpaned) );
  //gl_hpaned_pos = gtk_paned_get_position( GTK_PANED (hpaned) );

  //cout << gl_vpaned_pos << " -- " << gl_hpaned_pos << endl;

  /*Values printin out place */
  /*vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 2);*/


  /*aux_view = gtk_text_view_new ();
  gtk_text_view_set_editable(GTK_TEXT_VIEW (aux_view), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (aux_view), GTK_WRAP_WORD);

  gtk_container_add (GTK_CONTAINER (frame_vert), aux_view);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (aux_view));*/
  /*gtk_text_buffer_create_tag (buffer, "blue_foreground", "foreground", "blue", NULL);
  gtk_text_buffer_create_tag (buffer, "right_justify", "justification", GTK_JUSTIFY_RIGHT, NULL);
  gtk_text_buffer_create_tag (buffer, "left", "justification", GTK_JUSTIFY_LEFT, NULL);

  gtk_text_buffer_create_tag (buffer, "blue2_foreground", "foreground", "AliceBlue", NULL);
  gtk_text_buffer_create_tag (buffer, "orange_foreground", "foreground", "tan1", NULL);
  gtk_text_buffer_create_tag (buffer, "green_foreground", "foreground", "SpringGreen3", NULL);
  gtk_text_buffer_create_tag (buffer, "gray_foreground", "foreground", "gray86", NULL);

  gtk_text_buffer_create_tag (buffer, "little_big",
         /* points times the PANGO_SCALE factor /
         "size", 30 * PANGO_SCALE, NULL);*/


  //Main info view
  /*frame_vert = gtk_frame_new (NULL);//"MDT:");
  gtk_widget_set_valign (frame_vert, GTK_ALIGN_START);
  gtk_widget_set_size_request(frame_vert, 1, 187);
  gtk_box_pack_start(GTK_BOX (vbox), frame_vert, FALSE, FALSE, 15);

  view = gtk_text_view_new ();
  gtk_text_view_set_editable(GTK_TEXT_VIEW (view), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);*/

  /*buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
  gtk_text_buffer_create_tag (buffer, "left", "justification", GTK_JUSTIFY_LEFT, NULL);
  gtk_text_buffer_create_tag (buffer, "right_justify", "justification", GTK_JUSTIFY_RIGHT, NULL);
  gtk_text_buffer_create_tag (buffer, "big_gap_before_line", "pixels_above_lines", 30, NULL);
  gtk_text_buffer_create_tag (buffer, "big_gap_after_line", "pixels_below_lines", 30, NULL);
  gtk_text_buffer_create_tag (buffer, "wide_margins",  "left_margin", 50, "right_margin", 50, NULL);
  gtk_text_buffer_create_tag (buffer, "double_spaced_line", "pixels_inside_wrap", 10, NULL);

  gtk_text_buffer_create_tag (buffer, "blue_foreground", "foreground", "LightSkyBlue", NULL);
  gtk_text_buffer_create_tag (buffer, "red_foreground", "foreground", "red", NULL);

  gtk_text_buffer_create_tag (buffer, "orange_foreground", "foreground", "goldenrod1"/*"OrangeRed1"/, NULL);
  gtk_text_buffer_create_tag (buffer, "lightseagreen_foreground", "foreground", "aquamarine2", NULL);
  gtk_text_buffer_create_tag (buffer, "lightgray_foreground", "foreground", "gray86", NULL);

  gtk_text_buffer_create_tag (buffer, "big", "font-desc", font_desc,
         /* points times the PANGO_SCALE factor /
         "size", 70 * PANGO_SCALE, NULL);

  gtk_text_buffer_create_tag (buffer, "another_big",
         /* points times the PANGO_SCALE factor /
         "size", 70 * PANGO_SCALE, NULL);

  gtk_text_buffer_create_tag (buffer, "little_big",
         /* points times the PANGO_SCALE factor /
         "size", 30 * PANGO_SCALE, NULL);*/

 //gtk_container_add (GTK_CONTAINER (frame_vert), view);
  //gtk_box_pack_start(GTK_BOX (hbox), view, TRUE, TRUE, 6);


 GdkGeometry geomtry_hints;
 //geomtry_hints.min_width   = 100;
 //geomtry_hints.min_height  = 290;
 geomtry_hints.max_width   = 690;
 geomtry_hints.max_height  = 290;
 //geomtry_hints.base_height = -1;
 //geomtry_hints.base_width  = -1;

 //gtk_window_set_default_size(GTK_WINDOW(window), gWndWidth, gWndHeight);//623, 290);
 //gtk_widget_set_size_request(GTK_WIDGET(window), gWndWidth, gWndHeight);
 //gtk_window_set_geometry_hints(GTK_WINDOW(window), NULL, &geomtry_hints, (GdkWindowHints)(GDK_HINT_MAX_SIZE));
 gtk_window_set_title(GTK_WINDOW(window), buf_name); // "GTester 0.96.1");
 gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);

 myPixbuf = gdk_pixbuf_from_pixdata(&GTesterLogo, FALSE, NULL);

 //gdk_threads_add_timeout( 500, cb_timeout, (gpointer)buffer );

 /* Create a vertical box with buttons */
 frame_vert = gtk_frame_new (NULL);//"Controls:");
 gtk_widget_set_halign (frame_vert, GTK_ALIGN_START);
 gtk_box_pack_end(GTK_BOX (hbox), frame_vert, FALSE, TRUE, 10);

 overlay = gtk_overlay_new ();
 gtk_container_add (GTK_CONTAINER (frame_vert), overlay);

 //vbox = gtk_vbox_new (TRUE, 7);
 vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
 gtk_container_add (GTK_CONTAINER (overlay), vbox);
 //gtk_container_add (GTK_CONTAINER (frame_vert), vbox);

 button = gtk_button_new_with_mnemonic ("_Connect");
 g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (connect2DB), (gpointer) button);
 is_DB_connected == 0 ? gtk_widget_set_sensitive (button, FALSE) : gtk_widget_set_sensitive (button, TRUE);
 gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, TRUE, 0);

 query_button = gtk_button_new_with_mnemonic("Qu_ery F3");
 g_signal_connect (G_OBJECT (query_button), "clicked", G_CALLBACK (do_query), nullptr);
 gtk_box_pack_start (GTK_BOX (vbox), query_button, FALSE, TRUE, 0);
 gtk_widget_set_sensitive(query_button, enable_all_controlls);

 update_button = gtk_button_new_with_mnemonic("_Update F5");
 g_signal_connect (G_OBJECT (update_button), "clicked", G_CALLBACK (do_update), nullptr);
 gtk_box_pack_start (GTK_BOX (vbox), update_button, FALSE, TRUE, 0);
 gtk_widget_set_sensitive(update_button, enable_all_controlls);

 
 save_button = gtk_button_new_with_mnemonic("_Save F2");
 g_signal_connect (G_OBJECT (save_button), "clicked", G_CALLBACK (save_in_file_event), nullptr);
 gtk_box_pack_start (GTK_BOX (vbox), save_button, FALSE, TRUE, 0);

 insert_into_db_btn = gtk_button_new_with_mnemonic("_Insert F7");//"_Notify");
 //g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (parameters_accept), GTK_WINDOW(window) );
 g_signal_connect (G_OBJECT (insert_into_db_btn), "clicked", G_CALLBACK (insert_into_db_cb), GTK_WINDOW(window) );
 gtk_box_pack_start (GTK_BOX (vbox), insert_into_db_btn, FALSE, TRUE, 0);
 gtk_widget_set_sensitive(insert_into_db_btn, is_insert_into_db);

 delete_button = gtk_button_new_with_mnemonic("_Delete F8");//"_Notify");
 //g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (parameters_accept), GTK_WINDOW(window) );
 //gtk_widget_set_sensitive(button, is_insert_into_db);
 g_signal_connect (G_OBJECT (delete_button), "clicked", G_CALLBACK (delete_row_db_cb), GTK_WINDOW(window) );
 gtk_box_pack_start (GTK_BOX (vbox), delete_button, FALSE, TRUE, 0);
 gtk_widget_set_sensitive(delete_button, enable_all_controlls);
 

 clear_button = gtk_button_new_with_mnemonic("Clea_r Esc");//"_Notify");
 //g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (parameters_accept), GTK_WINDOW(window) );
 //gtk_widget_set_sensitive(button, is_insert_into_db);
 g_signal_connect (G_OBJECT (clear_button), "clicked", G_CALLBACK (clear_IO_patent_tree_cb), GTK_WINDOW(window) );
 gtk_box_pack_start (GTK_BOX (vbox), clear_button, FALSE, TRUE, 0);

 button = gtk_button_new_with_mnemonic("_About");
 g_signal_connect (button, "clicked", G_CALLBACK (about_us), GTK_WINDOW(window));
 gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 1);

 quit_button = gtk_button_new_with_mnemonic ("_Quit F12");
 g_signal_connect (quit_button, "clicked", G_CALLBACK(quit_app)/*gtk_main_quit*/, G_APPLICATION(data));
 gtk_box_pack_start (GTK_BOX (vbox), quit_button, FALSE, TRUE, 0);

 //transliterate_toggle = gtk_check_button_new_with_mnemonic("_Transliterate");
 //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(transliterate_toggle), is_transliterate);
 //g_signal_connect (G_OBJECT(transliterate_toggle), "toggled", G_CALLBACK (toggle_transliterate_cb), NULL);
 //gtk_box_pack_start(GTK_BOX (vbox), transliterate_toggle, FALSE, FALSE, 0);

 /*what_table_model = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
 gtk_list_store_append (what_table_model, &iter);
 gtk_list_store_set (what_table_model, &iter,
                          0, "Table One",
                          1, 1, -1);
 gtk_list_store_append (what_table_model, &iter);
 gtk_list_store_set (what_table_model, &iter,
                          0, "Table Two",
                          1, 2, -1);*/
 //gtk_list_store_append (what_table_model, &iter);
  
 /*combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (what_table_model));

 cell_renderer = gtk_cell_renderer_text_new ();
 gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo),
                                  cell_renderer,
                                  TRUE);
 gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo),
                                      cell_renderer,
                                      "text", 0,
                                      NULL);
 gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);

 g_signal_connect (G_OBJECT(combo), "changed", G_CALLBACK (combo_active_item_changed_cb), NULL);
 gtk_container_add (GTK_CONTAINER (vbox), combo);*/



 //gtk_box_pack_start (GTK_BOX (vbox), overlay, TRUE, TRUE, 0);

 //radio1 = gtk_check_button_new_with_mnemonic("Sound _mute");
 //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio1), sound_off);
 //g_signal_connect (G_OBJECT(radio1), "toggled", G_CALLBACK (toggle_snd_off), NULL);
 //gtk_box_pack_start(GTK_BOX (vbox), radio1, FALSE, FALSE, 0);


 label = gtk_label_new ("<span color=\"#F90101\">Archiver</span>");
 gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
 //gtk_label_set_text( GTK_LABEL (label), "F5 on/off");
 gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_CENTER);
 //gtk_box_pack_start(GTK_BOX (vbox), label, FALSE, FALSE, 0)
 gtk_widget_set_halign (label, GTK_ALIGN_CENTER);
 gtk_widget_set_valign (label, GTK_ALIGN_END);

 gtk_overlay_add_overlay (GTK_OVERLAY (overlay), label);
 gtk_widget_set_margin_left (label, 15);
 gtk_widget_set_margin_right (label, 5);
 gtk_widget_set_margin_top (label, 7);
 gtk_widget_set_margin_bottom (label, 23);

 //radio2 = gtk_check_button_new_with_mnemonic("Speaker o_ff");
 //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio2), speaker_off);
 //g_signal_connect (G_OBJECT(radio2), "toggled", G_CALLBACK (toggle_spkr_off), NULL);
 //gtk_box_pack_start(GTK_BOX (vbox), radio2, FALSE, FALSE, 0);

 //image = gtk_image_new_from_pixbuf(NULL);

 /* Enter the main loop */
 //gtk_window_present( GTK_WINDOW(window) );
 /*std::string _bg_color;
 _bg_color = "#000000000000";
 if (gdk_rgba_parse(&bg_color, (const gchar*)&_bg_color[0]))
 {
    gtk_widget_override_background_color(GTK_WIDGET(view), GTK_STATE_FLAG_NORMAL, &bg_color);
    gtk_widget_override_background_color(GTK_WIDGET(aux_view), GTK_STATE_FLAG_NORMAL, &bg_color);
 }*/

  //gtk_widget_modify_base (GTK_WIDGET(view), GTK_STATE_NORMAL, &bg_color);

 //gtk_widget_set_size_request (GTK_WIDGET (window), gWndWidth, gWndHeight);
 gtk_window_maximize(GTK_WINDOW(window));

 gtk_widget_show_all (GTK_WIDGET(window));

 const char file_name[] = "hello.txt";
 g_file_test(file_name, G_FILE_TEST_EXISTS) ? remove(file_name) : 0;

 return window;
}


void
open(GApplication *app, gpointer  user_data)
{
 GList *list;
 GtkWidget * gTesterWindow;

 list = gtk_application_get_windows ( GTK_APPLICATION(app) );
 if (list)
  gtk_window_present( GTK_WINDOW (list->data) );
 else
 {
  //Show_Splash_Screen("c://gt_splash.png" ,50, 25, 50, NULL);
  gTesterWindow = create_my_window(G_APPLICATION(app));
  gtk_window_set_application (GTK_WINDOW (gTesterWindow), GTK_APPLICATION(app));
  gtk_widget_show (gTesterWindow);
  MessageBox(alert_msg);
  //bclose_splash = true;
 }
}


static void
activate (GApplication *app, gpointer user_data)
{
  open(app, NULL);
}

void
startup(GApplication *app, gpointer user_data)
{
 /* Initialisation */
  person = new Person_List_Item();
  patent_list = g_array_sized_new (FALSE, FALSE, sizeof (Person_List_Item), 1);
}

void
app_shutdown(GApplication *app, gpointer user_data)
{
 //FinalizeDB();
  delete psys;
  delete foo;
  g_free(pCurrentDir);
  g_array_free (articles, FALSE);
  g_array_free (patent_list, FALSE);
}


int main (int argc, char *argv[])
{
  GtkWidget *button = NULL;
  GtkWidget *win = NULL;
  GtkWidget *vbox = NULL;

  psys = new CTasnif();
  foo =  new Irbis_Item();

  /* Initialize GTK+ */
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
  gtk_init (&argc, &argv);
  g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  /* Create the main window */
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (win), 8);
  gtk_window_set_title (GTK_WINDOW (win), "Hello World");
  gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  gtk_widget_realize (win);
  g_signal_connect (win, "destroy", gtk_main_quit, NULL);

  /* Create a vertical box with buttons */
  vbox = gtk_vbox_new (TRUE, 6);
  gtk_container_add (GTK_CONTAINER (win), vbox);

  button = gtk_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
  //g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (helloWorld), (gpointer) win);
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  g_signal_connect (button, "clicked", gtk_main_quit, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

  /* Enter the main loop */
  //gtk_widget_show_all (win);
  //gtk_main ();

  if ( g_application_id_is_valid ("tkl.mdt.GTester") )
    gApp = gtk_application_new ("tkl.mdt.GTester", G_APPLICATION_FLAGS_NONE);
   else
   {
     g_object_unref( gApp );
     return -1;
   }

 g_action_map_add_action_entries (G_ACTION_MAP (gApp),
           app_entries, G_N_ELEMENTS (app_entries),
           gApp);

 g_signal_connect (gApp, "startup", G_CALLBACK (startup), NULL);
 g_signal_connect (gApp, "shutdown", G_CALLBACK (app_shutdown), NULL);
 g_signal_connect (gApp, "activate", G_CALLBACK (activate), NULL);

 int status = g_application_run (G_APPLICATION (gApp), 0, NULL);

  g_object_unref( gApp );

  return 0;
}

int InitDB()
{
  //char *sql = NULL;
  //outf_data.open("out_data.txt", ios::out);

     try
     {
      /*connection C("dbname = ckbdb user = ckb password = ckbmdt \
      hostaddr = 127.0.0.1 port = 5432");*/
      conn2Postgres = new connection("dbname = ckbdb user = ckb password = ckbmdt \
                                            hostaddr = 127.0.0.1 port = 5432");

      //conn2Postgres = _conn2Postgres;

      if (conn2Postgres->is_open())
      {

        ifstream ifpar;
        ifpar.open("params.dat", ios::in);

        if(ifpar.is_open())
        {
           pCurrentDir = g_get_current_dir();
           std::string path_prefix;

           path_prefix.assign(pCurrentDir);

           pCmd.resize(2);
           for(short i=0; i < 2; i++)
           {
              //pCmd[i] = new char[128];
              getline(ifpar, pCmd[i]);

             //g_strstrip(pCmd[i]);
             //cout << i << ": " <<pCmd[i] << endl;
           }
           getline(ifpar, str); //json template file name

           if( pCmd[0].size() == 0 )
           {
             pCmd[0] = "ruby\\not\\found";   //ruby path
             pCmd[1] = "script\\not\\found"; // script
           }
      
          path_prefix += "/" + pCmd[1];
          pCmd[1] = path_prefix;
          
          ifpar.close();
         }
         //else
         //  cout << "not opened" << "\n";


        //ifstream ifs;
        ifpar.open(str/*"test_2.txt"*/, ios::in);

        //Json::Reader reader;
        //Json::Value obj;

        string s;
        char str2[64];

        gchar **tokens;
        int num_tokens, i;

        //reader.parse(ifs, obj);
        //reader.parse("{\"one\":1,\"two\":2,\"three\":3}", obj);

        patent_fields_name2.resize(2);

        ifpar.seekg(0, ifpar.beg);

        if (ifpar.is_open())
        {
          //int myints[]= {1,2,3,4,5,};
          //std::set<int> numbers (myints,myints+5);

          while(!ifpar.eof())
          {
            //ifs.read(str2, 32); 
            ifpar.getline(str2, 64, ':');
          
            //ifs >> str;
            //cout << str2 << endl;
            g_strdelimit(str2, "\"{}, ", ' ');
            g_strstrip(str2);
            
            //str.assign(str2);
            //if(isNumeric(str) == false)
             //cout << str2 << endl;

            patent_fields_name2[0].push_back(str2);

            ifpar.getline(str2, 64);
            //for(i = 1; i < num_tokens; i++){}
          }
        }

         //for (Json::Value::const_iterator it=obj.begin(); it!=obj.end(); ++it)
         //{
           //patent_fields_name2[0].push_back(it.key().asString());
           //cout << it.key().asString() << endl;
         //}
         
         /*patent_fields_name2[0].push_back("firstName");
         patent_fields_name2[0].push_back("midName");
         patent_fields_name2[0].push_back("lastName");

         patent_fields_name2[1].push_back("firstName2");
         patent_fields_name2[1].push_back("midName2");
         patent_fields_name2[1].push_back("lastName2");*/

         //cout << patent_fields_name2[0][2] << endl;

        ifpar.close();

        
         std::cout << "Database opened successfully: " << conn2Postgres->dbname() << std::endl;
         //fill_list_ckb_personnel();
         //prodName=new gchar[WORD_SIZE];
         outf_data.open("list.txt", ios::out);
         if (!outf_data.fail())
          return 0;
      } 
      else 
      {
         //cout << "Can't open database" << endl;
         return -1;
      }

      //sql = "select id, ckb_person->>'firstName' from ckb_persons as name WHERE \
               CAST ( ckb_person->>'age' AS INTEGER ) > 45";
      //sql = "SELECT * FROM ckb_persons where ckb_person->>'firstName' = 'Ibroxim'";
      //sql = "SELECT * FROM ckb_persons WHERE CAST( ckb_person->>'birth_date' AS INTEGER  ) = 1955";
      //nontransaction N(C);
      //result R( N.exec(sql) );

      /*for(result::const_iterator c = R.begin(); c != R.end(); ++c)
      {
        cout << c[0].as<int>() << endl;
	    cout << c[1].as<string>()<<endl;
      }*/

      //C.disconnect();
   } catch (const std::exception &e)
   {
      alert_msg.assign(e.what());
      cerr << alert_msg << std::endl;
      cerr << "error DB!" << std::endl;
      return -1;
   }

  return -1;
}
