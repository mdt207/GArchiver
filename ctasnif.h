//					Bismillahir Rahmanir Raheem
// All praises and gratitudes are due to Allah subhanahu wa Ta'ala
// O Allah: (please do) bless Muhammad (s.a.w) and the Household of Muhammad (s.a.w)

/*************************************************************\
* Muqobil Dasturlar To'plami (c)2009, 2010, 2011, 2012, 2017 *
* Purpose:Visual classifier                                  *
* Date:                                                      *
* authors:                                                   *
* original code by TKL K96 207                               *
\*************************************************************/
/* O'zbekiston jumhuriyati, Toshkent shahri, 2008-2017*/

#include <gtk/gtk.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


using namespace std;

class CTasnif
{
    public:
    CTasnif();
    ~CTasnif();

    void process(vector<string> &vdata);
    void process( char *s, short indx=0);

    void save_marked_cards(int);

    bool set_cards_files( vector<string> &l_cards_files);
    bool set_cards_files( vector<string> &l_cards_files, bool from_1_file);
    bool set_corrected_irbis_card( string &corr_card) { list_of_marked[_card_indx] = corr_card; return true; };
    bool set_card_text(char *s);
    int  set_card_index(int index) { list_of_marked.clear(); this->_card_indx = index; list_of_marked.push_back("~"); return _card_indx; }

    const char*    get_marks( int indx);
    int            get_marks_num();
    vector<string> get_cards_file_name();
    vector<string> get_irbis_name();
    vector<string> get_irbis_data();
    int            get_irbis_data_num () const { return list_of_marked.size(); };
    char*          get_sys_mes() const;
    char*          get_card_text() const;
    int            get_cards_amount() const { return nCards; };

    char* load_next_card( int card_indx);

    bool is_1_card( vector<string> &);

    private:

    char *_Matn,  *_message;
    char closure[2];
    ifstream in_card;
    int _card_indx, norm_card_num;
    int nCards;
    bool _from_1_file;

    vector<string> list_of_cards, list_of_irbis, list_of_marks, list_of_marked;
    vector<string> list_of_cards_from_1_file;
    vector<string> norm_card;
};
