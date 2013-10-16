#include <vector>
using namespace std;

bool findudp(vector <string> arregloudp, string numudp){

	for(int i=0;i<arregloudp.size();i++){
		if(arregloudp[i]==numudp)
			return true; //FUE VISTO
	}
return false; //NO FUE VISTO
}
