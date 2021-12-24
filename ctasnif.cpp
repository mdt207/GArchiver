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

//#include <algorithm>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cstring>
/*#include <istream>
#include <ostream>*/

#include "ctasnif.h"

//using namespace std;

CTasnif::CTasnif():
         //norm_card(13),
         _from_1_file(false),
         nCards(0)
  {
    _Matn   = new char[1024];
   _message = new char[255];

   memset( _Matn, 0, strlen(_Matn) );//universal

	strcpy(_message, "Кўриш учун белгини олиб ташланг.");

        list_of_marked.reserve(1);
        list_of_irbis.reserve(1);
        list_of_marks.reserve(1);

    ifstream fio;
    //char str[128];
    string str;
    string s;
    norm_card_num = 0;
    //string::size_type npos = 0;

      fio.open("data.dat");
      if(!fio.fail())
      {
        //
         while(fio >> s)
         {
           norm_card_num++;
           list_of_marks.push_back(s.append(" ") );
           //fio.getline(str, 128);
           getline(fio, str);
           g_strstrip(reinterpret_cast<gchar*>(&str[0]));
           //g_strchug( (gchar*)str );//rem leading white_spaces
           //g_strchomp( (gchar*)str);//rem trailing white_spaces
           list_of_irbis.push_back(str);
           //cout << s << str << endl;
         }
      }
      fio.close();

      norm_card.reserve(norm_card_num);

        /*list_of_irbis.push_back("автор");
        list_of_irbis.push_back("заглавие");
        list_of_irbis.push_back("издательство");
        list_of_irbis.push_back("библиографический");
        list_of_irbis.push_back("соавтор");
        list_of_irbis.push_back("ББК");
        list_of_irbis.push_back("УДК");*/


        /*list_of_marks.push_back("#1");
        list_of_marks.push_back("#2");
        list_of_marks.push_back("#3");
        list_of_marks.push_back("#4");
        list_of_marks.push_back("#5");
        list_of_marks.push_back("#6");
        list_of_marks.push_back("#7");*/
        //list_of_marks.push_back("#");

        s.resize(1);
        strcpy(closure, s.c_str());

        list_of_marks.push_back(closure);
        list_of_irbis.push_back("closure label");

        //cout << list_of_marks.capacity() << endl;
        //norm_card.reserve(list_of_marks.capacity()+2);
        //cout << "Norm_card cap:" <<norm_card.capacity() <<endl;
  }

CTasnif::~CTasnif()
  {
    delete [] _Matn;
    delete [] _message;
  }

void CTasnif::process(vector<string> &vdata)
 {
     string str;
      //concatenating norm_card
    for(int i=0; i <= vdata.size()-1 ; i++)
    {
       if( !vdata[i].empty() )
              str += vdata[i] + "\n";
              /*str += vdata[i];
              str +='\r'; //CR
              str +='\n'; //LF*/
       // str += norm_card[i];
    }
    //str += "\n";
    str.insert( str.size(), "*****\n" ); // DOS & WNT

    //UNIXEN
    /*str.insert( str.size(), "*****" );
    str +='\r';
    str +='\n';*/
    //cout << "process2:" << str << endl;


    //list_of_marked.push_back(str);
    //_card_indx = 1;//overcome seg.fault err
    list_of_marked[_card_indx] = str;
 }

void CTasnif::process( char *s, short indx)
  {
    string str;//temp. var.
    norm_card.clear();
    norm_card.resize(norm_card_num);
    //vector<string> norm_card(10);

    //ofstream ofile("view.txt", ios::out);

    /*Tokenize*/
    gchar** tokens;
    short num_tokens, i;
    int nseq = 0;

    tokens = g_strsplit( (const gchar *)s, closure, -1);
    num_tokens = g_strv_length(tokens);
    //cout << "Tokens:" << num_tokens << endl;


    //norm_card.reserve(num_tokens);

    //if( !ofile.fail() )
     {
         string snum;
         for( i=1; i < num_tokens-1; i++)
         {
             snum.assign(tokens[i]);
             //cout << snum << " ";
             snum.resize(2);

             if( g_ascii_isalnum(snum[0]) )
             {
                nseq = (int)strtol(snum.c_str(), (char**)NULL, 10);//atoi(snum.c_str());
                //nseq == 2 ? g_strdelimit(tokens[i], "\r\n", ' ') : NULL ; //irbis author tables record normalizing
                g_strdelimit(tokens[i], "\r\n", ' '); //normalizing all records of irbis table

                //cout <<  nseq << " " << tokens[i][0] << endl;
                //g_strchug( tokens[i] );//rem leading white_spaces
                //g_strchomp( tokens[i]);//rem trailing white_spaces
                g_strstrip(tokens[i]);
                //cout <<  nseq << " " << tokens[i] << endl;
                norm_card[nseq-1] += closure ; norm_card[nseq-1] += tokens[i]; //tokens[i][0] = ' ';
              }
           /*switch(tokens[i][0])
           {
             case '1':{ norm_card[0] += "#" ; norm_card[0] += tokens[i]; tokens[i][0] = ' '; }break;
             case '2':{ norm_card[1] += "#" ; norm_card[1] += tokens[i]; tokens[i][0] = ' '; }break;
             case '3':{ norm_card[2] += "#" ; norm_card[2] += tokens[i]; tokens[i][0] = ' '; }break;
             case '4':{ norm_card[3] += "#" ; norm_card[3] += tokens[i]; tokens[i][0] = ' '; }break;
             case '5':{ norm_card[4] += "#" ; norm_card[4] += tokens[i]; tokens[i][0] = ' '; }break;
             case '6':{ norm_card[5] += "#" ; norm_card[5] += tokens[i]; tokens[i][0] = ' '; }break;
             case '7':{ norm_card[6] += "#" ; norm_card[6] += tokens[i]; tokens[i][0] = ' '; }break;
           }*/
         }
         num_tokens = i;//for concatenating norm_card
         //cout << "Num_tokens:" << num_tokens << endl;
         //ofile << tokens[i];
         /*cout << "Norm:" << norm_card[0] << endl;
         cout << "Norm:" << norm_card[1] << endl;
         cout << "Norm:" << norm_card[2] << endl;
         cout << "Norm:" << norm_card[3] << endl;
         cout << "Norm:" << norm_card[4] << endl;
         cout << "Norm:" << norm_card[5] << endl;
         cout << "Norm:" << norm_card[6] << endl;*/
     }

    //concatenating norm_card
    for( i=0; i <= norm_card.size()-1; i++)
    {
       if( !norm_card[i].empty() )
       {
            str += norm_card[i];// + '\r';
            str += '\n';
            //str += 0x00A;
            //str += 0x00D;
       }
       // str += norm_card[i];
    }
    str += "*****\n";//"*****\r\n";
    //str.insert( str.size(), "*****\n" );
    //cout << "Card index:" << _card_indx << endl;


    //list_of_marked.push_back(str);
    //_card_indx = 1;//overcome seg.fault err
    list_of_marked[_card_indx] = str;
    //ofile << list_of_marked[_card_indx];
    //cout << list_of_marked[_card_indx] << endl;

    g_strfreev( tokens );
    //ofile.close();
 }
//0: 1 shot; 1: to be continued; 2: to be concluded
void CTasnif::save_marked_cards( int _whichStep )
 {
    //ofstream ofile("temp.txt", ios::out);
    ofstream ofile;
    string _closure;

    switch( _whichStep )
    {
        case 0: ofile.open("temp.txt", ios::out); _closure = "TKLK96207"; break;
        case 1: ofile.open("temp.txt", ios::app); /*_closure = '\n';*/        break;
        case 2: ofile.open("temp.txt", ios::app); /*_closure = "TKLK96207";*/ break;
    }

    //isSessionRestore ? ofile.open("temp.txt", ios::app) : ofile.open("temp.txt", ios::out);

    if( !ofile.fail() )
     {
       for( int i = 0; i < list_of_marked.size(); i++)
       {
         //cout << "i: " << i << endl;
         if( list_of_marked[i] != "~" ) ofile << list_of_marked[i];
       }
     }

   ofile << _closure;
   //isSessionRestore ? ofile << endl : ofile << "TKLK96207";
   ofile.close();
 }

bool CTasnif::set_cards_files( vector<string> &l_cards_files)
  {
    int c;

    _from_1_file = false;

    //list_of_marked.resize(0);
    list_of_marked.clear();
    list_of_cards.clear();

    list_of_cards = l_cards_files;
    vector<string>::const_iterator siter;

    for( c=0,siter = list_of_cards.begin(); siter != list_of_cards.end(); siter++, c++ )
    {
	    //cout << *siter << endl;
        list_of_marked.push_back("~");
    }
    nCards = c;

    in_card.open("junk.txt", ios::in);
    if( !in_card.fail() )
    {
        in_card.close();
        list_of_cards.push_back("junk.txt");
        list_of_marked.push_back("~");
    }
    //else cout << "The junk file not found" << endl;

    //cout << "Size:" << size << endl;

    /*cout << "Capacity: " <<list_of_cards.capacity() << endl; list_of_cards.capacity()
    list_of_marked.reserve(size);*/

    //cout << "Cap:" << list_of_marked.capacity() << endl;
    return true;
  }

  bool CTasnif::set_cards_files( vector<string> &l_cards_files, bool from_1_file)
  {
    string str, tmp, card_file_name;
    string::size_type pos = 0;
    vector<string> v;

    _from_1_file = from_1_file;

    list_of_cards_from_1_file.clear();
    list_of_marked.clear();
    list_of_cards.clear();

    //cout << "One whole file: " << l_cards_files[0] << endl;

    in_card.open(l_cards_files[0].c_str(), ios::in);
    if(!in_card.fail())
     {
     char achar[32];
     card_file_name = l_cards_files[0];
     copy(istream_iterator<string>(in_card), istream_iterator<string>(), back_inserter(v));
     nCards = count(v.begin(), v.end(), "*****");
     v.clear();
     //cout <<  "nCards:" << nCards << endl;
     in_card.clear(); in_card.seekg(ios::beg);

     gchar *gbuf;
     gbuf = g_path_get_basename( &(l_cards_files[0])[0]); //obtain pure file name

     card_file_name.assign(gbuf);
     //cout << l_cards_files[0] << endl;
     pos = card_file_name.rfind('.');

     g_free(gbuf);

     while(!in_card.eof())
     {
       getline(in_card, tmp);
       //g_strdelimit(&tmp[0],"\r",' ');
        //cout << tmp << endl;
        //str += str;

       //if(tmp.find_first_of("*****") != string::npos)//(tmp == "*****" || tmp == "*****\r")
       if(strcmp(tmp.c_str(), "*****") == 0)
       {
         //memset( _Matn, 0, strlen(_Matn) );//universal
         //cout << tmp << endl;
         //cout << str;
         str += "*****\n";

         //strcpy(_Matn, &str[0]);
         //list_of_cards_from_1_file.push_back(_Matn);

         list_of_cards_from_1_file.push_back(str);
         str.erase();
         //tmp.erase();
       }
       else
        {
            str += (tmp + "\n");
        }

     }
     in_card.clear(); in_card.close();

     for( int i=0; i<nCards; i++)
     {
        snprintf( achar, sizeof(achar), "%d", i+1);
        str = card_file_name;
        str.insert(pos, achar);
        //cout << achar << " "<< str << endl;
        list_of_cards.push_back(str);

        list_of_marked.push_back("~");
     }


     //gbuf = g_path_get_basename( &(l_cards_files[1])[0]); //obtain pure file name
     //g_free(gbuf);

      //cout <<"l_cards_files[1]" << l_cards_files[1] << endl;
      ifstream inf(&(l_cards_files[1])[0]/*"junk.txt"*/, ios::in);
      if( !inf.fail() )
      {
        //cout <<"l_cards_files[1]" << l_cards_files[1].c_str() << endl;
        getline(inf, tmp);
        //in_card.getline(&tmp[0], 100);
        list_of_cards_from_1_file.push_back(tmp);
        //in_card.close();

        list_of_cards.push_back("junk.txt");
        list_of_marked.push_back("~");
      }
      inf.close();
      //cout <<"l_cards_files[1]" << l_cards_files[1].c_str() << endl;
      //else cout << "The junk file not found" << endl;
	}

	return true;
  }

bool CTasnif::is_1_card( vector<string> &l_cards_files)
{
    bool res = true;
    vector< std::string > v;

    ifstream inpf;
    inpf.open(l_cards_files[0].c_str(), ios::in);

	if(!inpf.fail())
	{
     copy(istream_iterator<string>(inpf), istream_iterator<string>(), back_inserter(v));
     int nCards = count(v.begin(), v.end(), "*****");
     nCards > 1 ? res = false : res;
	}

    return res;
}

const char * CTasnif::get_marks( int indx)
 {
   return list_of_marks[indx].c_str();
 }

int CTasnif::get_marks_num()
 {
   return list_of_marks.size();
 }

vector<string> CTasnif::get_cards_file_name()
 {
   return  list_of_cards;
 }

vector<string> CTasnif::get_irbis_name()
 {
   return  list_of_irbis;
 }

 vector<string> CTasnif::get_irbis_data()
 {
   return norm_card;
 }

char* CTasnif::get_sys_mes() const
{
    return _message;
}

char* CTasnif::get_card_text() const
{
    /*if(strlen(_Matn) < 3 )
        return _Matn;*/
    return _Matn;
}

bool CTasnif::set_card_text(char *s)
{
    //memset( _Matn, 0, sizeof(_Matn) ); //clean it up, to be empty
    if(memcpy( _Matn, s, strlen(s)+1 ) != NULL)
    //if(strcpy(_Matn, s) != NULL)
        return true;
    else
        return false;
}

char* CTasnif::load_next_card( int card_indx)
  {
    //cout << &(list_of_cards_from_1_file[card_indx])[0] << endl;
    //char c_str[255];
    string str; //(255u,' ');
    _card_indx = card_indx;
    //cout << "_card_indx:" <<_card_indx << endl;
    if(_from_1_file)
    {
      //cout << &(list_of_cards_from_1_file[card_indx])[0] << endl;
      //memset( _Matn, 0, strlen(_Matn) );//universal
      strcpy(_Matn, &(list_of_cards_from_1_file[card_indx])[0]);
      //_Matn = &(list_of_cards_from_1_file[card_indx])[0];
      //cout << _Matn << endl;

      return _Matn;
      //return &(list_of_cards_from_1_file[card_indx])[0];
    }
    else
    {
      in_card.open(list_of_cards[card_indx].c_str(), ios::in);

      if( !in_card.fail())
      {
        //bzero(_Matn, strlen(_Matn));//UNICES
        memset( _Matn, 0, strlen(_Matn) );//universal

       /*in_card.getline(str, 100);
       str[sizeof(str)-1] = '\0';
       //strlcpy(_Matn, str, sizeof(str));
       _Matn = str.c_str();*/

       //cout << _Matn << endl;

        while( !in_card.eof())
        {
           //strcat(_Matn, " ");
           getline(in_card, str);
           str += "\n";
           //str = c_str;
           //in_card >> str;
           //str+="\t";
           strcat(_Matn, &str[0]);

           //cout << "str:"<<str << endl;
          //strncat(_Matn, str, sizeof(_Matn) - 1 - strlen(_Matn));

          //str[sizeof(str)-2] = '\0';
          //str[sizeof(str)-1] = 0xA;
          //strlcat(_Matn, str, sizeof(str));

          //strcat(_Matn, str);
          //cout << "Matn:"<<_Matn<<endl;
           //_Matn[strlen(str)] = '\0';
        }
      }

      //str+=" ";
      //strcat(_Matn, &str[0]);
      in_card.clear();
      in_card.close();
      //cout << _Matn;
      return _Matn;
    }
  }
