#include <vector>
using namespace std;

bool find(vector <string> arreglo, string num){

	for(int i=0;i<arreglo.size();i++){
		if(arreglo[i]==num)
			return true; //FUE VISTO
	}
return false; //NO FUE VISTO
}
