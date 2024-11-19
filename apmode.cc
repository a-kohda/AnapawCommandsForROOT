#include <stdio.h>
#include <vector>
#include <TRint.h>
#include <string.h>

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

	// このプログラムが置かれてる場所を取得
	// 参考:https://ken-ohwada.hatenadiary.org/entry/2021/01/03/120707
	char path[1024];
	readlink(Form("/proc/%d/exe",getpid()), path, 1024);
	//printf("link:%s\n",path);
	// 最後の / 以降を消してディレクトリにする
	char *p;
	p = strrchr(path,'/');
	if(p!=NULL) *(p+1)='\0';
	//printf("dir:%s\n",path);

	TRint::ProcessLine(Form(".L %sanapawcmd.C",path));
	TRint::ProcessLine("SetAPStyle();");

}

// Longptr_t 
Long_t TRint_apcr::ProcessLine(const char *line, Bool_t sync, Int_t *err){
	// ROOT6ではlineに#から始まる無意味な行が挿入されるので
	// それを取り除く。
	TString tsline(line);
	TString newtsline = tsline;
	if( tsline(0) == '#' ){
		int fpn = tsline.First('\n');
		newtsline = tsline( fpn+1, tsline.Length()-(fpn+1) );
	}

	// 入力文字列をスペース区切りのブロックに分割する。
	std::vector<TString> tsblocks;
	int pspace = 0; // tslineでの現在位置
	while(true){
		int npspace = ((TString)newtsline(pspace,newtsline.Length()-pspace)).First(' ');
		// npspaceはtslineでpspaceを起点に次の空白が何文字目にあるか
		if(npspace == 0){
			pspace++;
			continue;
		}
		if(npspace <0){
			tsblocks.push_back( (TString)newtsline(pspace,newtsline.Length()-pspace) );
			break;
		}
		tsblocks.push_back( (TString)newtsline(pspace,npspace) );
		pspace += npspace + 1;
	}
	
//	for(int i=0;i<tsblocks.size(); i++){
//		printf("%s\n", tsblocks[i].Data());
//	}


// 1番目の文字列ブロックがコマンドと一致すれば関数として整形
	bool IsAPcmd = false;
	if(tsblocks.size()>=1){
		IsAPcmd += tsblocks[0].EqualTo("ls");
		IsAPcmd += tsblocks[0].EqualTo("ht");
		IsAPcmd += tsblocks[0].EqualTo("hn");
		IsAPcmd += tsblocks[0].EqualTo("hb");
		IsAPcmd += tsblocks[0].EqualTo("hlist");
		IsAPcmd += tsblocks[0].EqualTo("lnx");
		IsAPcmd += tsblocks[0].EqualTo("lgx");
		IsAPcmd += tsblocks[0].EqualTo("lny");
		IsAPcmd += tsblocks[0].EqualTo("lgy");
		IsAPcmd += tsblocks[0].EqualTo("lnz");
		IsAPcmd += tsblocks[0].EqualTo("lgz");
		IsAPcmd += tsblocks[0].EqualTo("sly");
		IsAPcmd += tsblocks[0].EqualTo("chbin");
		IsAPcmd += tsblocks[0].EqualTo("blowx");
		IsAPcmd += tsblocks[0].EqualTo("blowy");
		IsAPcmd += tsblocks[0].EqualTo("mami");
		IsAPcmd += tsblocks[0].EqualTo("prx");
		IsAPcmd += tsblocks[0].EqualTo("pry");
		IsAPcmd += tsblocks[0].EqualTo("delstat");
		IsAPcmd += tsblocks[0].EqualTo("showstat");
		IsAPcmd += tsblocks[0].EqualTo("hupdate");
		IsAPcmd += tsblocks[0].EqualTo("print");
		IsAPcmd += tsblocks[0].EqualTo("zone");
		IsAPcmd += tsblocks[0].EqualTo("cdn");
		IsAPcmd += tsblocks[0].EqualTo("size");
		IsAPcmd += tsblocks[0].EqualTo("figali");
		IsAPcmd += tsblocks[0].EqualTo("adl");
		IsAPcmd += tsblocks[0].EqualTo("blow");
		IsAPcmd += tsblocks[0].EqualTo("htofunc");
		IsAPcmd += tsblocks[0].EqualTo("mail");
		IsAPcmd += tsblocks[0].EqualTo("fitsgl");
		IsAPcmd += tsblocks[0].EqualTo("divide");
		IsAPcmd += tsblocks[0].EqualTo("fls");
		IsAPcmd += tsblocks[0].EqualTo("fit");
		IsAPcmd += tsblocks[0].EqualTo("fc");
		IsAPcmd += tsblocks[0].EqualTo("info");
		IsAPcmd += tsblocks[0].EqualTo("title");
		IsAPcmd += tsblocks[0].EqualTo("set");
		IsAPcmd += tsblocks[0].EqualTo("zonesize");
	}

// 2番目以降の文字列ブロックが
// 数値以外の文字を含む場合に前後にクオーテーションを付ける
	if(IsAPcmd){
		for(int i=1;i<tsblocks.size();i++){
			if(  ! tsblocks[i].IsFloat() ){
				tsblocks[i] = Form("\"%s\"",tsblocks[i].Data());
			}
		}
	}


	TString formedAPcmd;
	if(tsblocks.size()>=1 && IsAPcmd ){
		TString tstemp2;
		tstemp2 = tsblocks[0];
		if( tsblocks[0].EqualTo("set") || tsblocks[0].EqualTo("zonesize") ) tstemp2 = "APCR_" + tsblocks[0];
		tstemp2 += "(";
		for(int i=0;i<tsblocks.size()-1; i++){
			if(i>0)tstemp2 += ",";
			tstemp2 += tsblocks[i+1];
		}
		tstemp2 += ");";
		//printf("%s\n", tstemp2.Data());
		formedAPcmd = tstemp2;
	}

	if(IsAPcmd){
		TRint::ProcessLine(formedAPcmd.Data(), kFALSE, err);
	}else{
		TRint::ProcessLine(tsline.Data(), kFALSE, err);
	}
	return 1;	
}


