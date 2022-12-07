// ANAPAW Commands for ROOT
// Version 0.9
// Last Updated 2022.12.7 by A. Kohda 


// グローバル変数の定義
TString defaultdrawopt = "colz"; 
vector<double> gDoubleVec;

// ANAPAW準拠のユーザー用関数
// 基本操作用の関数
void hlist();                // Index番号付きでヒストグラムのリストを表示する
void (*ls)() = hlist;        // "hlist()"は"ls()"でも可
void ht(int n, TString opt); // n番目のhistをDraw (オプション指定あり)
void ht(TString opt);        // 現在表示されているhistのoptionを変更して再Draw
void hn(TString opt);        // 現在表示されているhistの次のhistを表示
void hb(TString opt);        // 現在表示されているhistの前のhistを表示

// フィッティング系
//void fig();                  // 現在表示されているhistをGausianでfit (現在表示中の全範囲で)
//void xfitg();                // マウスで最大、最小を指定してGausianでfit
//void fitgl(float xmin, float xmax); // gaus+直線でfit
//void xfitgl();
//void fiterf(float xmin, float xmax); // 誤差関数でfit

// 表示調整系
void lgx();                  // x軸をログスケールにする
void lnx();                  // x軸をリニアスケールにする
void lgy();                  // y軸をログスケールにする
void lny();                  // y軸をリニアスケールにする
void lgz();                  // z軸をログスケールにする
void lnz();                  // z軸をリニアスケールにする

// 2次元 -> 1次元
//void sly(int n = -1);        // 2次元ヒストグラムをスライスしてy軸への射影

// オリジナルのユーザー用関数
TH1*  htp(int n = -1);        // HID指定、または現在表示されているhistのポインタを返す
//void  xrange(float xmin, float xmax);
//void  xrange();


// このマクロ内で使用する関数
TH1* GetCurrentHist(bool quiet);// 現在表示されているhistのポインタを返す
int  GetObjID(TObject* o1);  // ヒスト等のオブジェクトがリストの何番目にあるかを返す
TList* GetHistList();        // .lsで表示されるオブジェクトのリスト
void DrawHist(TH1* h1, TString opt);
//void DeleteObjFromGPad(TString name); // gPad上にある(TGraphなどの)オブジェクトを消す(nameで指定)


///////// 関数・クラスの実体記述部 //////////


TH1* GetCurrentHist(bool quiet = false){
	TH1* h1;
	if( gPad == 0x0 ){
		h1 = 0x0;
		if(!quiet) fprintf(stderr, "Warning! No current Histgram !\n");
	}else{
		h1 = (TH1*)gPad->GetListOfPrimitives()->At(1);
	}
	return h1;
}

int GetObjID(TObject* o1){
	TList* li = GetHistList();
	int cidx = -1;
	cidx = li->IndexOf(o1);
	return cidx; 
}

void hlist(){
	printf("\n  ===> Histogram List\n\n");
	printf("    HID    Kind    Name\n\n");

	TList* li = GetHistList();
	int cidx = GetObjID(GetCurrentHist(true));
	TString arrow;


	for(int n=0;n < li->GetEntries();n++){
		int kind=9;
		TObject* obj = li->At(n);
		if(obj->InheritsFrom("TH3")) { kind = 3; }
		else if(obj->InheritsFrom("TH2")) { kind = 2; }
		else if(obj->InheritsFrom("TH1")) { kind = 1; }
		if ( n == cidx ) { arrow = "->"; }
		else { arrow = "  "; }
		printf(" %s %3d    (%d)    %s\n", arrow.Data(), n, kind, obj->GetName());
	}
}

void ht(int n, TString opt = defaultdrawopt){
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->At(n);
	DrawHist(h1, opt);
}

void ht(TString opt){
	TH1* h1 = (TH1*)htp();
	if(h1 == 0x0) return;
	DrawHist(h1, opt);
}

void DrawHist(TH1* h1, TString opt = defaultdrawopt){
	// なぜか2回目にDrawした時にstat boxのサイズ(y方向のみ)が変わってしまうのを防ぐ措置
	TPaveStats *st = (TPaveStats*)h1->FindObject("stats");
	bool  statexist = false;
	float staty1ndc;
	if(st!=0x0){
		staty1ndc = st->GetY1NDC();
		statexist = true;
	}	
	h1->UseCurrentStyle(); // style を強制適用
	if(gPad == 0x0) TCanvas *c1 = new TCanvas(); // キャンバスがない場合、生成
	if(statexist) st->SetY1NDC(staty1ndc);
	h1->Draw(opt);
	printf(" Draw ID:%3d  %s\n",GetObjID(h1),h1->GetName());
}

void lnx(){ gPad->SetLogx(0); }
void lgx(){ gPad->SetLogx(1); }
void lny(){ gPad->SetLogy(0); }
void lgy(){ gPad->SetLogy(1); }
void lnz(){ gPad->SetLogz(0); }
void lgz(){ gPad->SetLogz(1); }


