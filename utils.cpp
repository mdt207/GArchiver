#include <cctype>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "utils.h"


std::map<std::string, std::string> Lat2Lat =
{
    /*std::pair<std::string, std::string>("Щ","Ŝ"),
    std::pair<std::string, std::string>("Ж","Ž"),
    std::pair<std::string, std::string>("Ё","Ë"),
    std::pair<std::string, std::string>("Ў","Ŭ"),
    std::pair<std::string, std::string>("Х","X"),*/
    std::pair<std::string, std::string>("a","a"),
    std::pair<std::string, std::string>("b","b"),
    std::pair<std::string, std::string>("v","v"),
    std::pair<std::string, std::string>("g","g"),
    std::pair<std::string, std::string>("d","d"),
    std::pair<std::string, std::string>("e","ye"),
    std::pair<std::string, std::string>("u","u"),
    std::pair<std::string, std::string>("j","j"),
    std::pair<std::string, std::string>("ë","yo"),
    std::pair<std::string, std::string>("ŭ","o'"),
    std::pair<std::string, std::string>("ž","zh"),
    std::pair<std::string, std::string>("z","z"),
    std::pair<std::string, std::string>("i","i"),
    std::pair<std::string, std::string>("k","k"),
    std::pair<std::string, std::string>("l","l"),
    std::pair<std::string, std::string>("m","m"),
    std::pair<std::string, std::string>("n","n"),
    std::pair<std::string, std::string>("o","o"),
    std::pair<std::string, std::string>("p","p"),
    std::pair<std::string, std::string>("r","r"),
    std::pair<std::string, std::string>("s","s"),
    std::pair<std::string, std::string>("t","t"),
    std::pair<std::string, std::string>("f","f"),
    std::pair<std::string, std::string>("x","x"),
    std::pair<std::string, std::string>("ț","c"),
    std::pair<std::string, std::string>("č","ch"),
    std::pair<std::string, std::string>("š","sh"),
    std::pair<std::string, std::string>("ŝ","shch"),
    std::pair<std::string, std::string>("ǎ","\""),
    std::pair<std::string, std::string>("y","y"),
    std::pair<std::string, std::string>("ь","\'"),
    std::pair<std::string, std::string>("è","e"),
    std::pair<std::string, std::string>("û","yu"),
    std::pair<std::string, std::string>("â","ya"),
    std::pair<std::string, std::string>("ğ","g'"),
    std::pair<std::string, std::string>("q","q"),
    std::pair<std::string, std::string>("h","h"),
	
    std::pair<std::string, std::string>("A","A"),
    std::pair<std::string, std::string>("B","B"),
    std::pair<std::string, std::string>("V","V"),
    std::pair<std::string, std::string>("G","G"),
    std::pair<std::string, std::string>("D","D"),
    std::pair<std::string, std::string>("E","E"),
    std::pair<std::string, std::string>("U","U"),
    std::pair<std::string, std::string>("J","J"),
    std::pair<std::string, std::string>("Ë","Yo"),
    std::pair<std::string, std::string>("Ŭ","O'"),
    std::pair<std::string, std::string>("Ž","Zh"),
    std::pair<std::string, std::string>("Z","Z"),
    std::pair<std::string, std::string>("I","I"),
    std::pair<std::string, std::string>("K","K"),
    std::pair<std::string, std::string>("L","L"),
    std::pair<std::string, std::string>("M","M"),
    std::pair<std::string, std::string>("N","N"),
    std::pair<std::string, std::string>("O","O"),
    std::pair<std::string, std::string>("P","P"),
    std::pair<std::string, std::string>("R","R"),
    std::pair<std::string, std::string>("S","S"),
    std::pair<std::string, std::string>("T","T"),
    std::pair<std::string, std::string>("F","F"),
    std::pair<std::string, std::string>("X","X"),
    std::pair<std::string, std::string>("Ț","C"),
    std::pair<std::string, std::string>("Č","Ch"),
    std::pair<std::string, std::string>("Š","Sh"),
    std::pair<std::string, std::string>("Ŝ","Shch"),
    std::pair<std::string, std::string>("Ǎ","\""),
    std::pair<std::string, std::string>("Y","Y"),
    std::pair<std::string, std::string>("Ь","\'"),
    std::pair<std::string, std::string>("È","E"),
    std::pair<std::string, std::string>("Û","Yu"),
    std::pair<std::string, std::string>("Â","Ya"),
    std::pair<std::string, std::string>("Ğ","G'"),
    std::pair<std::string, std::string>("Q","Q"),
    std::pair<std::string, std::string>("H","H")
};


std::map<std::string, std::string> Cyr2Lat =
{
    /*std::pair<std::string, std::string>("Щ","Ŝ"),
    std::pair<std::string, std::string>("Ж","Ž"),
    std::pair<std::string, std::string>("Ё","Ë"),
    std::pair<std::string, std::string>("Ў","Ŭ"),
    std::pair<std::string, std::string>("Х","X"),*/
    std::pair<std::string, std::string>("а","a"),
    std::pair<std::string, std::string>("б","b"),
    std::pair<std::string, std::string>("в","v"),
    std::pair<std::string, std::string>("г","g"),
    std::pair<std::string, std::string>("д","d"),
    std::pair<std::string, std::string>("е","e"),
    std::pair<std::string, std::string>("у","u"),
    std::pair<std::string, std::string>("й","j"),
    std::pair<std::string, std::string>("ё","ë"),
    std::pair<std::string, std::string>("ў","ŭ"),
    std::pair<std::string, std::string>("ж","ž"),
    std::pair<std::string, std::string>("з","z"),
    std::pair<std::string, std::string>("и","i"),
    std::pair<std::string, std::string>("к","k"),
    std::pair<std::string, std::string>("л","l"),
    std::pair<std::string, std::string>("м","m"),
    std::pair<std::string, std::string>("н","n"),
    std::pair<std::string, std::string>("о","o"),
    std::pair<std::string, std::string>("п","p"),
    std::pair<std::string, std::string>("р","r"),
    std::pair<std::string, std::string>("с","s"),
    std::pair<std::string, std::string>("т","t"),
    std::pair<std::string, std::string>("ф","f"),
    std::pair<std::string, std::string>("х","x"),
    std::pair<std::string, std::string>("ц","ț"),
    std::pair<std::string, std::string>("ч","č"),
    std::pair<std::string, std::string>("ш","š"),
    std::pair<std::string, std::string>("щ","ŝ"),
    std::pair<std::string, std::string>("ъ","ǎ"),
    std::pair<std::string, std::string>("ы","y"),
    std::pair<std::string, std::string>("ь","ь"),
    std::pair<std::string, std::string>("э","è"),
    std::pair<std::string, std::string>("ю","û"),
    std::pair<std::string, std::string>("я","â"),
    std::pair<std::string, std::string>("ғ","ğ"),
    std::pair<std::string, std::string>("қ","q"),
    std::pair<std::string, std::string>("ҳ","h"),
	
    std::pair<std::string, std::string>("А","A"),
    std::pair<std::string, std::string>("Б","B"),
    std::pair<std::string, std::string>("В","V"),
    std::pair<std::string, std::string>("Г","G"),
    std::pair<std::string, std::string>("Д","D"),
    std::pair<std::string, std::string>("Е","E"),
    std::pair<std::string, std::string>("У","U"),
    std::pair<std::string, std::string>("Й","J"),
    std::pair<std::string, std::string>("Ё","Ë"),
    std::pair<std::string, std::string>("Ў","Ŭ"),
    std::pair<std::string, std::string>("Ж","Ž"),
    std::pair<std::string, std::string>("З","Z"),
    std::pair<std::string, std::string>("И","I"),
    std::pair<std::string, std::string>("К","K"),
    std::pair<std::string, std::string>("Л","L"),
    std::pair<std::string, std::string>("М","M"),
    std::pair<std::string, std::string>("Н","N"),
    std::pair<std::string, std::string>("О","O"),
    std::pair<std::string, std::string>("П","P"),
    std::pair<std::string, std::string>("Р","R"),
    std::pair<std::string, std::string>("С","S"),
    std::pair<std::string, std::string>("Т","T"),
    std::pair<std::string, std::string>("Ф","F"),
    std::pair<std::string, std::string>("Х","X"),
    std::pair<std::string, std::string>("Ц","Ț"),
    std::pair<std::string, std::string>("Ч","Č"),
    std::pair<std::string, std::string>("Ш","Š"),
    std::pair<std::string, std::string>("Щ","Ŝ"),
    std::pair<std::string, std::string>("Ъ","Ǎ"),
    std::pair<std::string, std::string>("Ы","Y"),
    std::pair<std::string, std::string>("Ь","Ь"),
    std::pair<std::string, std::string>("Э","È"),
    std::pair<std::string, std::string>("Ю","Û"),
    std::pair<std::string, std::string>("Я","Â"),
    std::pair<std::string, std::string>("Ғ","Ğ"),
    std::pair<std::string, std::string>("Қ","Q"),
    std::pair<std::string, std::string>("Ҳ","H")
};


void CPathBuilder::trimStr(std::string &s)
{
	//std::string aString("    This is a string to be trimmed   ");
	if(s.empty()) return;
	
	auto start = s.find_first_not_of(' ');
	auto end = s.find_last_not_of(' ');
	std::string trimmedString;
	s.substr(start, (end - start) + 1).swap(s);
}

std::string CPathBuilder::getBuiltPath()
{
	return _built_path;
}

int CPathBuilder::setBase(std::string const &base_of_path)
{
	_base_path.assign(base_of_path);
	trimStr(_base_path);
	//_built_path.assign(base_of_path);
	return 0;
}

int CPathBuilder::appendTrailer(std::string &tail_of_path)
{
	//_built_path.append(tail_of_path);
	_built_path.assign(_base_path);
	//_built_path.clear();
	trimStr(tail_of_path);
	if(tail_of_path[0] != '\\')
	{
		std::string tmp("\\");
		tmp.append(tail_of_path);
		_built_path.append(tmp);
	}
	else
		_built_path.append(tail_of_path);
	//cout << "setTail:" << _built_path << endl;
	return 0;
}

template<typename V>
bool CTransliter::is_utf8_char(V val)
{
    auto ch = val;
    //int cc = ch;
    if ( isascii(ch) )
	return false;
    else
	return true;
}

/*
 * Generic implementation to search if a given value exists in a map or not.
 * Adds all the keys with given value in the vector
 */
template <typename K, typename V>
bool CTransliter::findByValue(std::vector<K> & vec, std::map<K, V> mapOfElemen, V value)
{
	bool bResult = false;
	auto it = mapOfElemen.begin();
	// Iterate through the map
	while(it != mapOfElemen.end())
	{
		//cout << it->first ;
		// Check if value of this entry matches with given value
		if(it->second == value)
		{
			// Yes found
			bResult = true;
			// Push the key in given map
			vec.push_back(it->first);
		}
		// Go to next entry in map
		it++;
	}
	return bResult;
}


std::string CTransliter::getLat2Lat(std::string &what)
{
    //char buf[] = {'j','J','e','E'};
    _str_result.clear();
    std::string str;
    //std::set<char> exception_sym(buf, buf+4);

    //std::cout << what << std::endl;
    //cout << getCyr2Lat(Cyr2Lat[pStr]) << endl;
    //_str_result = what;
    for(int i=0; i < what.length(); i+=1)
    {
	 if ( is_utf8_char(what[i]))// || exception_sym.count(what[i]) )
         {
            str = Lat2Lat[what.substr(i,2)];
            //std::cout << i << "in CTrans get: " << str << std::endl;
		   _str_result.append(str);
		   ++i;
		 }
		 else
		 {
             str = Lat2Lat[what.substr(i,1)];
			 //str = std::string(1, what[i]); 
			_str_result.append(str);
		 }
     }
  return _str_result;
}


std::string CTransliter::getCyr2Lat(std::string &what)
{
    _str_result.clear();
    std::string str;

    //std::cout << what << std::endl;
    //cout << getCyr2Lat(Cyr2Lat[pStr]) << endl;
    //_str_result = what;
    for(int i=0; i < what.length(); i+=1)
    {
	 if ( is_utf8_char(what[i]) )
         {
            str = Cyr2Lat[what.substr(i,2)];
            //std::cout << i << "in CTrans get: " << str << std::endl;
		   _str_result.append(str);
		   ++i;
		 }
		 else
		 {
			str = std::string(1, what[i]); 
			_str_result.append(str); 
		 }
     }
  return _str_result;
}

std::string CTransliter::getLat2Cyr(std::string &what)
{
	   _str_result.clear();
	   bool res = false;
	   int i=0;
	   std::vector<std::string> vec;

	   //std::cout<<"Keys with value "<< what << " are, ";

	   for( ; i < what.length(); i+=1)
	   {
	     //res = this->findByValue(vec, Cyr2Lat, what.substr(i,2));
             if ( is_utf8_char(what[i]) )
             {
			res = this->findByValue(vec, Cyr2Lat, what.substr(i,2));
			++i;
		 }
		 else
		 {
			 res = this->findByValue(vec, Cyr2Lat, std::string(1, what[i]));
		 }
	   } 
	   if(res)
		 {
			 //std::cout<<"Keys with value "<< what << " are,"<<std::endl;
			 for(auto elem : vec)
			 {
				 _str_result.append(elem);
		 	 	 //std::cout<<_char_val;
		 	 }
		 }
		 /*else
		 {
		    	//cout << what <<" not found" <<endl;
	 	 }*/
	 	std::cout << _str_result << "\n";
	   return _str_result;
}

   
std::string UnTrans2(std::string str, bool real_trans_into_cyr)
{
    std::stringstream sb;
    /*var sb = Pool<StringBuilder>.Get();

    if (sb == null)
    {
        sb = new StringBuilder();
    }
    else
    {
      sb.Clear();
    }*/

            for (int i = 0; i < str.length(); i++)
            {
                auto itm = str[i];

                switch (itm)
                {
                        // lower
                    case 's':
                    {

                        if (i + 2 < str.length())
                        {
                            if (str[i + 1] == 'h')
                            {
                                if (str[i + 2] == 'c' && str[i + 3] == 'h')
                                {
                                    real_trans_into_cyr ? sb << ("щ") : sb <<  ("ŝ");

                                    i += 3;

                                    break;
                                }

                                i += 1;

                                real_trans_into_cyr ? sb << ("ш") : sb <<  ("š");

                                break;
                            }

                            real_trans_into_cyr ? sb << ("с") : sb <<  ("s");

                            break;
                        }

                        if (i + 1 < str.length())
                        {
                            if (str[i + 1] == 'h')
                            {
                                i += 1;

                                real_trans_into_cyr ? sb << ("ш") : sb <<  ("š");

                                break;
                            }
                        }

                        real_trans_into_cyr ? sb << ("с") : sb <<  ("s");

                        break;
                    }
                    case 'y': 
                    {
                        if (i + 1 < str.length())
                        {
                            switch (str[i+1])
                            {
                                case 'a':
                                {
                                    i += 1;

                                    real_trans_into_cyr ? sb << ("я") : sb <<  ("â");

                                    continue;
                                }
                                case 'e':
                                {
                                    i += 1;

                                    real_trans_into_cyr ? sb << ("е") : sb <<  ("e");

                                    continue;
                                }
                                case 'u':
                                {
                                    i += 1;

                                    real_trans_into_cyr ? sb << ("ю") : sb <<  ("û");

                                    continue;
                                }
                                case 'o':
                                {
                                    i += 1;

                                    real_trans_into_cyr ? sb << ("ё") : sb <<  ("ë");

                                    continue;
                                }
                            }
                        }

                        real_trans_into_cyr ? sb << ("ы") : sb <<  ("y");

                        break;
                    }
                    case 'z':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'h')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("ж") : sb <<  ("ž");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("з") : sb <<  ("z");

                        break;
                    }
                    case 'k':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'h')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("х") : sb <<  ("x");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("к") : sb <<  ("k");

                        break;
                    }
                    case 'i':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'y')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("й") : sb <<  ("j");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("и") : sb <<  ("i");

                        break;
                    }
                    case 'c':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'h')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("ч") : sb <<  ("č");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("ц") : sb <<  ("ț");

                        break;
                    }
                    case 'f':
                    {
                        real_trans_into_cyr ? sb << ("ф") : sb <<  ("f");

                        break;
                    }
                    case 'u':
                    {
                        real_trans_into_cyr ? sb << ("у") : sb <<  ("u");

                        break;
                    }
                    case 't':
                    {
                        real_trans_into_cyr ? sb << ("т") : sb <<  ("t");

                        break;
                    }
                    case 'r':
                    {
                        real_trans_into_cyr ? sb << ("р") : sb <<  ("r");

                        break;
                    }
                    case 'p':
                    {
                        real_trans_into_cyr ? sb << ("п") : sb <<  ("p");

                        break;
                    }
                    case 'o':
                    {
                        if (i + 1 < str.length() && str[i + 1] == '\'')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("ў") : sb <<  ("ŭ");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("о") : sb <<  ("o");

                        break;
                    }
                    case 'n':
                    {
                        real_trans_into_cyr ? sb << ("н") : sb <<  ("n");

                        break;
                    }
                    case 'm':
                    {
                        real_trans_into_cyr ? sb << ("м") : sb <<  ("m");

                        break;
                    }
                    case 'l':
                    {
                        real_trans_into_cyr ? sb << ("л") : sb <<  ("l");

                        break;
                    }
                    case 'd':
                    {
                        real_trans_into_cyr ? sb << ("д") : sb <<  ("d");

                        break;
                    }
                    case 'g':
                    {
                        if (i + 1 < str.length() && str[i + 1] == '\'')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("ғ") : sb <<  ("ğ");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("г") : sb <<  ("g");

                        break;
                    }
                    case 'v':
                    {
                        real_trans_into_cyr ? sb << ("в") : sb <<  ("v");

                        break;
                    }
                    case 'b':
                    {
                        real_trans_into_cyr ? sb << ("б") : sb <<  ("b");

                        break;
                    }
                    case 'a':
                    {
                        real_trans_into_cyr ? sb << ("а") : sb <<  ("a");

                        break;
                    }
                    case '’':
                    case '\'':
                    {
                        real_trans_into_cyr ? sb << ("ь") : sb <<  ("ь");

                        break;
                    }
                    case '”':
                    case '\"':
                    {
                        real_trans_into_cyr ? sb << ("ъ") : sb <<  ("ǎ");

                        break;
                    }
                    case 'e':
                    {
                        real_trans_into_cyr ? sb << ("э") : sb <<  ("è");

                        break;
                    }
                    case 'q':
                    {
                       real_trans_into_cyr ? sb << ("қ") : sb <<  ("q");

                       break;
                    }
                    case 'h':
                    {
                       real_trans_into_cyr ? sb << ("ҳ") : sb <<  ("h");
                       break; 
                    } 

                        // upper
                    case 'S':
                    {
                        if (i + 3 < str.length())
                        {
                            if (str[i + 1] == 'h')
                            {
                                if (str[i + 2] == 'c' && str[i + 3] == 'h')
                                {
                                    i += 3;

                                    real_trans_into_cyr ? sb << ("Щ") : sb <<  ("Ŝ");

                                    break;
                                }

                                i += 1;

                                real_trans_into_cyr ? sb << ("Ш") : sb <<  ("Š");

                                break;
                            }

                            real_trans_into_cyr ? sb << ("С") : sb <<  ("S");

                            break;
                        }

                        if (i + 1 < str.length())
                        {
                            if (str[i + 1] == 'h')
                            {
                                i += 1;

                                real_trans_into_cyr ? sb << ("Ш") : sb <<  ("Š");

                                break;
                            }
                        }

                        real_trans_into_cyr ? sb << ("С") : sb <<  ("S");

                        break;
                    }
                    case 'Y':
                    {
                        if (i + 1 < str.length())
                        {
                            switch (str[i + 1])
                            {
                                case 'a':
                                    {
                                        i += 1;

                                        real_trans_into_cyr ? sb << ("Я") : sb <<  ("Â");

                                        continue;
                                    }
                                case 'e':
                                    {
                                        i += 1;

                                        real_trans_into_cyr ? sb << ("Е") : sb <<  ("E");

                                        continue;
                                    }
                                case 'u':
                                    {
                                        i += 1;

                                        real_trans_into_cyr ? sb << ("Ю") : sb <<  ("Û");

                                        continue;
                                    }
                                case 'o':
                                    {
                                        i += 1;

                                        real_trans_into_cyr ? sb << ("Ё") : sb <<  ("Ë");

                                        continue;
                                    }
                            }
                        }

                        real_trans_into_cyr ? sb << ("Ы") : sb <<  ("Y");

                        break;
                    }
                    case 'Z':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'h')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("Ж") : sb <<  ("Ž");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("З") : sb <<  ("Z");

                        break;
                    }
                    case 'K':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'h')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("Х") : sb <<  ("X");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("К") : sb <<  ("K");

                        break;
                    }
                    case 'I':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'y')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("Й") : sb <<  ("J");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("И") : sb <<  ("I");

                        break;
                    }
                    case 'C':
                    {
                        if (i + 1 < str.length() && str[i + 1] == 'h')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("Ч") : sb <<  ("Č");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("Ц") : sb <<  ("Ț");

                        break;
                    }
                    case 'F':
                    {
                        real_trans_into_cyr ? sb << ("Ф") : sb <<  ("F");

                        break;
                    }
                    case 'U':
                    {
                        real_trans_into_cyr ? sb << ("У") : sb <<  ("U");

                        break;
                    }
                    case 'T':
                    {
                        real_trans_into_cyr ? sb << ("Т") : sb <<  ("T");

                        break;
                    }
                    case 'R':
                    {
                        real_trans_into_cyr ? sb << ("Р") : sb <<  ("R");

                        break;
                    }
                    case 'P':
                    {
                        real_trans_into_cyr ? sb << ("П") : sb <<  ("P");

                        break;
                    }
                    case 'O':
                    {
                      if (i + 1 < str.length() && str[i + 1] == '\'')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("Ў") : sb <<  ("Ŭ");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("О") : sb << ("O");

                        break;
                    }
                    case 'N':
                    {
                        real_trans_into_cyr ? sb << ("Н") : sb <<  ("N");

                        break;
                    }
                    case 'M':
                    {
                        real_trans_into_cyr ? sb << ("М") : sb <<  ("M");

                        break;
                    }
                    case 'L':
                    {
                        real_trans_into_cyr ? sb << ("Л") : sb <<  ("L");

                        break;
                    }
                    case 'D':
                    {
                        real_trans_into_cyr ? sb << ("Д") : sb <<  ("D");

                        break;
                    }
                    case 'G':
                    {
                        if (i + 1 < str.length() && str[i + 1] == '\'')
                        {
                            i += 1;

                            real_trans_into_cyr ? sb << ("Ғ") : sb <<  ("Ğ");

                            break;
                        }

                        real_trans_into_cyr ? sb << ("Г") : sb <<  ("G");

                        break;
                    }
                    case 'V':
                    {
                        real_trans_into_cyr ? sb << ("В") : sb <<  ("V");

                        break;
                    }
                    case 'B':
                    {
                        real_trans_into_cyr ? sb << ("Б") : sb <<  ("B");

                        break;
                    }
                    case 'A':
                    {
                        real_trans_into_cyr ? sb << ("А") : sb <<  ("A");

                        break;
                    }
                    case 'E':
                    {
                        real_trans_into_cyr ? sb << ("Э") : sb <<  ("È");

                        break;
                    }
                    case 'Q':
                    {
                       real_trans_into_cyr ? sb << ("Қ") : sb <<  ("Q");

                       break;
                    }
                    case 'H':
                    {
                       real_trans_into_cyr ? sb << ("Ҳ") : sb <<  ("H");
                       
                       break; 
                    } 

                    default :
                    {
                        sb << (itm);
 
                        break;
                    }
                }
            }

   //Pool<StringBuilder>.Release(sb);

   return sb.str();
}

bool isNumeric(std::string str) 
{
   for (int i = 0; i < str.length(); i++)
      if (isdigit(str[i]) == false)
      return false; //when one non numeric value is found, return false
   return true;
}