#include <stdio.h>
#include <vector>
#include <istream>
#include <sstream>
#include <TRint.h>
#include "TObjString.h"
#include <TROOT.h>

class TRint_apcr : public TRint{
	public:
		TRint_apcr(const char *appClassName, Int_t *argc, char **argv,
	   void *options=0, int numOptions=0, Bool_t noLogo = kFALSE);
		Long_t ProcessLine(const char *line, Bool_t sync = kFALSE, Int_t *error = nullptr);
};

int main(int argc, char* argv[]){
    
    TRint_apcr *app = new TRint_apcr("App", &argc, argv);
		app->SetPrompt("APCR> ");
    app->Run();
    return 0;
}

TRint_apcr::TRint_apcr(const char *appClassName, Int_t *argc, char **argv,
		   void *options, Int_t numOptions, Bool_t noLogo)
: TRint(appClassName, argc, argv, options, numOptions, kTRUE){
	TRint::ProcessLine(".L /home/kohda/.rootmacros/AnapawCommandsForROOT/anapawcmd.C");

}


Long_t TRint_apcr::ProcessLine(const char *line, Bool_t sync, Int_t *err){


////// copy from artemis sources/main/TArtRint.cc#L91
      TString lines(line);
      TObjArray *arr = lines.Tokenize('\n');
      std::vector<TString> cmds;
      for (Int_t i = 0; i < arr->GetEntries(); ++i) {
         TString aCmd = ((TObjString*)arr->At(i))->GetString();
         if (!aCmd.BeginsWith("#")) {
            cmds.push_back(aCmd);
         }
      }
      TString cmdline;
      for (Int_t i=0; i<cmds.size(); ++i) {
         if (i>0) cmdline.Append("\n");
         cmdline.Append(cmds[i]);
      }
//	printf("%s\n",cmdline.Data());
//////


// 入力文字列をスペース区切りで分割
	std::vector<TString> vts;
	std::string s = cmdline.Data();
	std::stringstream ss{s};
	std::string buf;
	while (std::getline(ss, buf, ' ')) {
		TString stemp = Form("%s",buf.c_str());
		if( !stemp.EqualTo("") )vts.push_back(stemp);
	}
//	for(int i=0;i<vts.size(); i++){
//		printf("%s\n", vts[i].Data());
//	}

// 1番目の文字列ブロックがコマンドと一致すれば関数として整形
	TString formedAPcmd;
	if(vts.size()>=1){
		TString tstemp2;
		tstemp2 = vts[0];
		tstemp2 += "(";
		for(int i=0;i<vts.size()-1; i++){
			if(i>0)tstemp2 += ",";
			tstemp2 += vts[i+1];
		}
		tstemp2 += ")";
		//printf("%s\n", tstemp2.Data());
		formedAPcmd = tstemp2;
	}

	bool IsAPcmd = false;
	if(vts.size()>=1){
		IsAPcmd += vts[0].EqualTo("ls");
		IsAPcmd += vts[0].EqualTo("ht");
		IsAPcmd += vts[0].EqualTo("hn");
		IsAPcmd += vts[0].EqualTo("hb");
		IsAPcmd += vts[0].EqualTo("hlist");
		IsAPcmd += vts[0].EqualTo("lnx");
		IsAPcmd += vts[0].EqualTo("lgx");
		IsAPcmd += vts[0].EqualTo("lny");
		IsAPcmd += vts[0].EqualTo("lgy");
		IsAPcmd += vts[0].EqualTo("lnz");
		IsAPcmd += vts[0].EqualTo("lgz");
		IsAPcmd += vts[0].EqualTo("sly");
		IsAPcmd += vts[0].EqualTo("chbin");
		IsAPcmd += vts[0].EqualTo("blowx");
		IsAPcmd += vts[0].EqualTo("blowy");
		IsAPcmd += vts[0].EqualTo("mami");
		IsAPcmd += vts[0].EqualTo("prx");
		IsAPcmd += vts[0].EqualTo("delstat");
		IsAPcmd += vts[0].EqualTo("showstat");
		IsAPcmd += vts[0].EqualTo("hupdate");
		IsAPcmd += vts[0].EqualTo("print");
		IsAPcmd += vts[0].EqualTo("zone");
		IsAPcmd += vts[0].EqualTo("size");
		IsAPcmd += vts[0].EqualTo("figali");
	}

	//printf("aa\n");
	TString sline(line);
//	//printf("%s\n",sline.Data());
//	if( sline.EqualTo("help") ){
//		printf("aaaaaaa\n");
//		sline = ".help";
//	}

	if(IsAPcmd){
		TRint::ProcessLine(formedAPcmd.Data(), kFALSE, err);
	}else{
		TRint::ProcessLine(sline.Data(), kFALSE, err);
	}
	return 1;	
}


