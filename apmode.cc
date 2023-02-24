#include <stdio.h>
#include <vector>
#include <istream>
#include <sstream>
#include <TRint.h>


class TRint_apcr : public TRint{
	public:
		TRint_apcr(const char *appClassName, Int_t *argc, char **argv,
	   void *options=0, int numOptions=0, Bool_t noLogo = kFALSE);
		Long_t ProcessLine(const char *line, Bool_t sync, Int_t *err);
};

int main(int argc, char* argv[]){
    
    TRint_apcr *app = new TRint_apcr("App", &argc, argv);
		app->SetPrompt("APCR> ");
    app->Run();
    return 0;
}

TRint_apcr::TRint_apcr(const char *appClassName, Int_t *argc, char **argv,
		   void *options, Int_t numOptions, Bool_t noLogo)
: TRint(appClassName, argc, argv, options, numOptions, kTRUE){}


Long_t TRint_apcr::ProcessLine(const char *line, Bool_t sync, Int_t *err){
// $BF~NOJ8;zNs$r%9%Z!<%96h@Z$j$GJ,3d(B
	std::vector<TString> vts;
	std::string s = line;
	std::stringstream ss{s};
	std::string buf;
	while (std::getline(ss, buf, ' ')) {
		TString stemp = Form("%s",buf.c_str());
		if( !stemp.EqualTo("") )vts.push_back(stemp);
	}
//	for(int i=0;i<vts.size(); i++){
//		printf("%s\n", vts[i].Data());
//	}

// 1$BHVL\$NJ8;zNs%V%m%C%/$,%3%^%s%I$H0lCW$9$l$P4X?t$H$7$F@07A(B
	if(vts.size()>=1){
		TString tstemp2;
		tstemp2 = vts[0];
		tstemp2 += "(";
		for(int i=0;i<vts.size()-1; i++){
			if(i>0)tstemp2 += ",";
			tstemp2 += vts[i+1];
		}
		tstemp2 += ")";
		printf("%s\n", tstemp2.Data());
	}

	//printf("aa\n");
	TString sline(line);
	//printf("%s\n",sline.Data());
	if( sline.EqualTo("help") ){
		printf("aaaaaaa\n");
		sline = ".help";
	}
	TRint::ProcessLine(sline.Data(), kFALSE, err);
	return 1;	
}


