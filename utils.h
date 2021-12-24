#include <set>
#include <vector>
#include <map>
#include <string>

class CPathBuilder
{
public:
	CPathBuilder(){}
	CPathBuilder(std::string const &base_of_path)
	{
		_base_path.assign(base_of_path);
		//cout << _built_path << "\n";
	}
	~CPathBuilder(){}
	int setBase(std::string const &base_of_path);
	int appendTrailer(std::string &tail_of_path);
	std::string getBuiltPath();
	void trimStr(std::string &s);
private:
  std::string _built_path, _base_path;
};

class CTransliter
{
 public:
   CTransliter(){}
   ~CTransliter(){}


   std::string getCyr2Lat(std::string &what);
   std::string getLat2Cyr(std::string &what);
   std::string getLat2Lat(std::string &what);

private:
   /*
    * Generic implementation to search if a given value exists in a map or not.
    * Adds all the keys with given value in the vector
    */
 template<typename K, typename V>
 bool findByValue(std::vector<K> & vec, std::map<K, V> mapOfElemen, V value);

private: 
   std::string _char_val;
   std::string _str_result;
   
private:
   template <typename V>
   bool is_utf8_char(V val);
};

std::string UnTrans2(std::string str, bool real_trans_into_cyr = true);
bool isNumeric(std::string str);
