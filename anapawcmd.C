// バージョン情報
void APCver(){
	printf("                                       \n");
	printf("  Welcome to ANAPAW Commands for ROOT  \n");
	printf("  This is Version 1.03                 \n");
	printf("  Last Updated 2023. 1.31 by A. Kohda  \n");
	printf("                                       \n");
}
//////////////////////////////////////////////////////

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
//TH1*  htp(int n = -1);        // HID指定、または現在表示されているhistのポインタを返す


// このマクロ内で使用する関数
TH1* GetCurrentHist(bool quiet);// 現在表示されているhistのポインタを返す
int  GetObjID(TObject* o1);  // ヒスト等のオブジェクトがリストの何番目にあるかを返す
TList* GetHistList();        // .lsで表示されるオブジェクトのリスト
void DrawHist(TH1* h1, TString opt);
//void DeleteObjFromGPad(TString name); // gPad上にある(TGraphなどの)オブジェクトを消す(nameで指定)
void SetAPStyle();           // Histの見栄えをANAPAWっぽいstyleにする
void CdNPad();

// 関数エイリアス
TH1* gH1(){ return GetCurrentHist(true); }

///////// 関数・クラスの実体記述部 //////////


TH1* GetCurrentHist(bool quiet = false){
	TH1* h1;
	if( gPad == 0x0 ){
		h1 = 0x0;
		if(!quiet) fprintf(stderr, " Warning ! No current Histgram !\n");
	}else{
		h1 = (TH1*)gPad->GetListOfPrimitives()->At(1);
	}
	return h1;
}

int GetObjID(TObject* o1){
	TList* li = GetHistList();
	int cidx = -1;
	cidx = li->IndexOf(o1);
	return cidx; // ない場合は -1 を返す(テスト済み)
}

TList* GetHistList(){
	TList *li = new TList();
	if(gDirectory->GetListOfKeys() != 0x0){
		for(int i=0; i< gDirectory->GetListOfKeys()->GetEntries(); i++){
			li->Add(gROOT->FindObject(gDirectory->GetListOfKeys()->At(i)->GetName()));
		}
	}

	for(int i=0; i< gROOT->GetList()->GetEntries(); i++){
		li->Add(gROOT->GetList()->At(i));
	}
	return li;
}

void hlist(){
	printf("\n  ===> Histogram List\n\n");
	printf("    HID    Kind    Title\n\n");

	TList* li = GetHistList();
	int cidx = GetObjID(GetCurrentHist(true));
	TString arrow;

	for(int n=0;n < li->GetEntries();n++){
		char kind='S';
		TObject* obj = li->At(n);
		if     (obj->InheritsFrom("TH3"))   { kind = '3'; }
		else if(obj->InheritsFrom("TH2"))   { kind = '2'; }
		else if(obj->InheritsFrom("TH1"))   { kind = '1'; }
		else if(obj->InheritsFrom("TTree")) { kind = 'T'; }
		if ( n == cidx ) { arrow = "->"; }
		else { arrow = "  "; }
		if (obj->InheritsFrom("TH1")){
			printf(" %s %3d    (%c)    %s\n", arrow.Data(), n, kind, obj->GetTitle());
		}else{
			printf(" %s %3d    (%c)    %s; %s\n", arrow.Data(), n, kind, obj->GetName(),obj->GetTitle());
		}
	}
}

void ht(int n, TString opt = defaultdrawopt){
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->At(n);
	if(h1 == 0x0 || ! h1->InheritsFrom("TH1") ){
		printf(" %d do not exist or not histgram\n", n);
		return;
	}
	DrawHist(h1, opt);
}

void ht(TString opt){
	TH1* h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	DrawHist(h1, opt);
}

void S_hNorB(int kind, TString opt = defaultdrawopt){
	if(kind != 1 && kind != 2) return;
	// kind 1:hn, 2:hb
	TH1* h1 = (TH1*)GetCurrentHist(true);
	int currentHID = GetObjID(h1); // リストにない場合は -1 が入るはず。
	TList* li = GetHistList();
	int linum = li->GetEntries();
	TH1* h1_2;
	bool exdraw = false;
	for(int i=0; i< linum+1 ; i++){
		int newHID = -1;
		if(kind==1) newHID = currentHID + 1;
		if(kind==2) newHID = currentHID - 1;
		h1_2 = (TH1*)li->At(newHID);

		if(h1_2 != 0x0 &&  h1_2 ->InheritsFrom("TH1") && newHID >= 0 ){ 
			exdraw = true;
			break; 
		}
		if(kind==1){
			if(currentHID < li->GetEntries() ){ currentHID ++; }
			else{ currentHID = -1; }
		}
		if(kind==2){
			if(currentHID > 0 ){ currentHID --; printf("test\n");}
			else{ currentHID = linum; }
		}
	}
	if(exdraw){ DrawHist(h1_2, opt); }
}

void hn(TString opt = defaultdrawopt){ S_hNorB(1, opt); }
void hb(TString opt = defaultdrawopt){ S_hNorB(2, opt); }


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
	CdNPad(); // キャンバス分割している場合は次のPadに移る。(なければしない)
	h1->SetXTitle(Form("Histogram ID = %d",GetObjID(h1)));
	h1->Draw(opt);
	printf(" Draw ID:%3d  %s\n",GetObjID(h1),h1->GetTitle());
  defaultdrawopt = opt;
}

void lnx(){ gPad->SetLogx(0); }
void lgx(){ gPad->SetLogx(1); }
void lny(){ gPad->SetLogy(0); }
void lgy(){ gPad->SetLogy(1); }
void lnz(){ gPad->SetLogz(0); }
void lgz(){ gPad->SetLogz(1); }

// ここから開発中関数
void S_slXorY(int axis, int div=-1, bool ira = false){ 
	// div : 分割数
	// ira : insert right after。ANAPAWのようにHIDを直後に挿入する場合true。
	//       ここでは既存のHIDを変更しないようにlistの最後に挿入するモードを
	//       デフォルトにしている。
	// axis 1:x, 2:y
	if(axis != 1 && axis != 2) {
		printf("Input Error!\n");
		return;
	}

	TH2D* h2 = (TH2D*)GetCurrentHist();
	if( h2==0x0 ) { return; }
	if( ! h2->InheritsFrom("TH2") ) { 
		printf(" not 2d hist\n");
		return;
	}
	TString ptitle = h2->GetTitle(); // TitleではなくNameを使う必要あり。要改善
	TString pname  = h2->GetName(); // TitleではなくNameを使う必要あり。要改善
	int binnum = h2->GetNbinsX(); // 引数指定がなければbin numで分割する
	TH1D* firsth1;
	for(int i=0;i<binnum;i++){
		while( gROOT->FindObject(Form("%s_sly%d",pname.Data(),i+1)) != 0x0 ) {
			pname += "_"; // nameが重複する場合"_"をもう一個足す。
		}
		TH1D* h1 = h2->ProjectionY(Form("%s_sly%d",pname.Data(),i+1),i+1,i+1);
		float xstart = h2->GetXaxis()->GetBinLowEdge(i+1);
		float xend   = xstart + h2->GetXaxis()->GetBinWidth(i+1);
		h1->SetTitle(Form("%s sly%d (x = %#6g : %#6g)",ptitle.Data(),i+1,xstart,xend));
		gDirectory->GetListOfKeys()->AddLast(h1);
		if(i==0) firsth1 = h1;
	}
	DrawHist(firsth1);
}

//void slx(){ S_slXorY(1); }
void sly(){ S_slXorY(2); }

void blowx(float xmin, float xmax){ // ANAROOT準拠。旧バージョンのxrange。
	TH1 *h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->GetXaxis()->SetRangeUser(xmin,xmax);
	gPad->Modified();
}

void blowx(){ // 引数なしバージョンはunzoom
	TH1 *h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->GetXaxis()->UnZoom();
	gPad->Modified();
}

void blowy(float min, float max){ // ANAROOT準拠。旧バージョンのxrange。
	TH1 *h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->GetYaxis()->SetRangeUser(min,max);
	gPad->Modified();
}

void blowy(){ // 引数なしバージョンはunzoom
	TH1 *h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->GetYaxis()->UnZoom();
	gPad->Modified();
}

void delstat(){
	TH1* h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->SetStats(0);
	gPad->Modified();
}

void showstat(){
	TH1* h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->SetStats(1);
	gPad->Modified();
}

void hupdate(){
	if(gPad == 0x0) return;
	gPad->Modified();
	gPad->Update();
}

void print(){
	if(gPad == 0x0) return;
	// 保存場所
	vector<TString> sdircand; // 保存ディレクトリの候補
	sdircand.push_back("./png");    // 候補順に書く
	sdircand.push_back("../png"); 
	sdircand.push_back("./plots"); 
	sdircand.push_back("../plots");
	//sdircand.push_back("./");

	bool saved = false;
	for(int i=0;i<sdircand.size();i++){
		char command[128];
		sprintf(command, "test -d %s", sdircand[i].Data()); // OS依存性あり
		//printf("%s\n",command);
		int returnval = system(command);
		//printf("%d\n",returnval);
		if(returnval == 0){
			TString fname = Form("%s/root%d_%d.png",
				sdircand[i].Data(),
				TDatime().GetDate(),TDatime().GetTime());

			//printf("%s\n",fname.Data());
			gPad->GetCanvas()->SaveAs(fname.Data());
			saved = true;
			break;
		}
	}
	if(!saved) printf(" Not saved because candidate directories not found.\n");
}

void zone(int x, int y){
	if(gPad == 0x0) TCanvas *c1 = new TCanvas();
	TH1* h1 = (TH1*)GetCurrentHist();
	gPad->GetCanvas()->Clear();
	if(x==1 && y==1){
		gPad->GetCanvas()->Clear();
	} else {
		gPad->GetCanvas()->Divide(x,y);
		gPad->GetCanvas()->cd(x*y); // DrawHist padnum+1するのでここはmax(苦肉の策)
	}
	if(h1 != 0x0) DrawHist(h1);
}

void CdNPad(){
	if(gPad == 0x0) return;
	if(gPad == gPad->GetCanvas()) return; // 分割されてなければ終了
	if(gPad == gPad->GetCanvas()->cd(gPad->GetNumber()+1) ){
		gPad->GetCanvas()->cd(1);	
	}
}

void size(float w, float h){ // デフォルトのサイズに対する比率で指定
	if(gPad == 0x0) return;
	int defw = 600;
	int defh = 600;
	int difw = 2;
	int difh = 24;
	gPad->GetCanvas()->SetWindowSize (w * defw + difw, h * defh + difh);
}

void SetAPStyle(){
	//int fontid=22; // Times系太字フォント(サイズは割合指定)
	int fontid=23; // Times系太字フォント(サイズはpx指定)
	float fsize = 18; // フォントサイズ(px)
	gStyle->SetStatFont(fontid);
	gStyle->SetLabelFont(fontid,"XYZ");
	gStyle->SetLabelFont(fontid,"");
	gStyle->SetTitleFont(fontid,"XYZ");
	gStyle->SetTitleFont(fontid,"");
	gStyle->SetTextFont(fontid);
	gStyle->SetLegendFont(fontid);
	if(1){
		gStyle->SetStatFontSize(fsize);
		gStyle->SetLabelSize(fsize, "XYZ");
		gStyle->SetLabelSize(fsize, "");
		gStyle->SetTitleSize(fsize, "XYZ");
		gStyle->SetTitleFontSize(fsize*1.2);
		gStyle->SetLegendTextSize(fsize);
	}
	gStyle->SetHistFillColor(7);
	gStyle->SetHistFillStyle(3001);
	gStyle->SetHistLineColor(kBlue);
	gStyle->SetFuncColor(kRed);
	gStyle->SetFrameLineWidth(2);
	gStyle->SetPadGridX(1);
	gStyle->SetPadGridY(1);
	gStyle->SetCanvasColor(0);
	gStyle->SetTitleFillColor(0);
	gStyle->SetTitleStyle(0);
	gStyle->SetStatColor(0);
	gStyle->SetStatStyle(0);
	gStyle->SetStatX(0.9);  
	gStyle->SetStatY(0.9);  
	gStyle->SetPalette(1);
	gStyle->SetOptLogz(1);
	gStyle->SetOptFit(1);
	gStyle->SetOptStat(1111111);
	gStyle->SetPadBorderMode(1);
	// 自分で追加
	gStyle->SetCanvasDefW(600+2);  // 指定した値と実際のサイズ(png保存したとき
	gStyle->SetCanvasDefH(600+24); // 画像サイズ)が違うので注意
	//gStyle->SetStatFontSize(0.025);
	gStyle->SetStatW(0.5); // Statの高さは、文字サイズと行数で自動で決まる
	gStyle->SetStatH(0.11); // px指定フォントの場合は影響されない?	
}

