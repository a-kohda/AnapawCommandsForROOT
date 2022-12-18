// ANAPAW Commands for ROOT
// Version 0.9
// Last Updated 2022.12.7 by A. Kohda 

// saho/sahoRIBF142/scripts/eucalib.cxx にも参考コードあり

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
void fig();                  // 現在表示されているhistをGausianでfit (現在表示中の全範囲で)
void xfitg();                // マウスで最大、最小を指定してGausianでfit
void fitgl(float xmin, float xmax); // gaus+直線でfit
void xfitgl();
void fiterf(float xmin, float xmax); // 誤差関数でfit

// 表示調整系
void lgx();                  // x軸をログスケールにする
void lnx();                  // x軸をリニアスケールにする
void lgy();                  // y軸をログスケールにする
void lny();                  // y軸をリニアスケールにする
void lgz();                  // z軸をログスケールにする
void lnz();                  // z軸をリニアスケールにする

// 2次元 -> 1次元
void sly(int n = -1);        // 2次元ヒストグラムをスライスしてy軸への射影

// オリジナルのユーザー用関数
TH1*  htp(int n = -1);        // HID指定、または現在表示されているhistのポインタを返す
void  xrange(float xmin, float xmax);
void  xrange();


// このマクロ内で使用する関数
TH1* GetCurrentHist();       // 現在表示されているhistのポインタを返す
int  GetObjID(TObject* o1);  // ヒスト等のオブジェクトがリストの何番目にあるかを返す
TList* GetHistList();        // .lsで表示されるオブジェクトのリスト
void DrawHist(TH1* h1, TString opt);
void DeleteObjFromGPad(TString name); // gPad上にある(TGraphなどの)オブジェクトを消す(nameで指定)
class GetXYbyClick {         // マウスを使って点の取得を行う
	private:
		int maxnum; // 取得する最大数
		TString name;
		vector<float> x; // 2変数を持つ構造体のvectorに変更したい
		vector<float> y;
	public:
		GetXYbyClick(TString aname);
		GetXYbyClick(TString aname, int maxnumber);
		void addpoints(int maxnumber);
		void execgetxy();
		void print();
		float GetXat(int n);
		float GetYat(int n);
		int GetN();
		void reset();
};

// マクロ内で使うグローバル変数
GetXYbyClick   globalxyclick("globalxyclick");


///////// 関数・クラスの実体記述部 //////////


void hlist(){
	//int n=5;
	printf("\n  ===> Histogram List\n\n");
	printf("    HID    Kind    Name\n\n");

	TList* li = GetHistList();
	int cidx = -1;
	TString arrow;
	if(gPad != 0x0 ){
		if(gPad->GetListOfPrimitives()->At(1) != 0x0 ){
			cidx = li->IndexOf(gPad->GetListOfPrimitives()->At(1));
		}
	}
	//printf("Current hist index is : %d\n",cidx);
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



///////////// 以下、未整理 ///////////////

// ubuntu14からのコピー

// ROOTのバージョン選択(使わない方をコメントアウト)
#define USINGROOT5
//#define USINGROOT6

#ifdef USINGROOT5
  #define nullptr 0x0
  #define INFINITY 99999
#endif

struct xvaldata {
	int maxnum;
	int curnum;
	float x[3];
	float y[3];
};
struct xvaldata globalxvaldata;

////



void xgetvalx(int n){
	printf("Please click %d points !\n",n);
	printf("   X       Y   \n");
	globalxvaldata.maxnum =n;
	globalxvaldata.curnum =0;
	gPad->AddExec( "xgetvalx2" , "xgetvalx2()" );
	gPad->WaitPrimitive();
//	printf("%f\t%f\n",xvaldata1.x[0], xvaldata1.y[0]);
//	printf("%f\t%f\n",xvaldata1.x[1], xvaldata1.y[1]);
//	printf("%f\t%f\n",xvaldata1.x[2], xvaldata1.y[2]);

}
void xgetvalx2(){
	if(gPad->GetEvent() == 1 ){
		printf("%#6g %#6g \n", gPad->AbsPixeltoX(gPad->GetEventX()), gPad->AbsPixeltoY(gPad->GetEventY()));
		//number++;
		globalxvaldata.x[globalxvaldata.curnum] = gPad->AbsPixeltoX(gPad->GetEventX());
		globalxvaldata.y[globalxvaldata.curnum] = gPad->AbsPixeltoY(gPad->GetEventY());
		globalxvaldata.curnum ++ ;

		if(globalxvaldata.curnum>=globalxvaldata.maxnum){
			gPad->DeleteExec("xgetvalx2");
			printf("Please double click for finish !\n");
		}
	}else{
		printf("%#6g %#6g \r", gPad->AbsPixeltoX(gPad->GetEventX()), gPad->AbsPixeltoY(gPad->GetEventY()));
	}
}








TH1* hn(TString opt){ // 現在表示されているhistの次のhistを表示
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->After(gPad->GetListOfPrimitives()->At(1));
	h1->SetOption(opt);
	h1->Draw();
	printf(" Draw ID:%3d  %s\n",li->IndexOf(h1),h1->GetName());
	return h1;
}

void hn(){ // ↑のオプション維持版
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->After(gPad->GetListOfPrimitives()->At(1));
//	h1->Draw();
//	printf(" Draw ID:%3d  %s\n",li->IndexOf(h1),h1->GetName());
	ht(li->IndexOf(h1));
	//return h1;
}

TH1* hb(TString opt){ // 現在表示されているhistの前のhistを表示
	TH1* h1 = (TH1*)GetHistList()->Before(gPad->GetListOfPrimitives()->At(1));
	h1->SetOption(opt);
	h1->Draw();
	return h1;
}

void hb(){ // ↑のオプション維持版
	TList* li = GetHistList();
	TH1* h1 = (TH1*)GetHistList()->Before(gPad->GetListOfPrimitives()->At(1));
	//h1->Draw();
	ht(li->IndexOf(h1));
//	return h1;
}

void figa(){ // 現在表示されているhistをGausianでfit (xrangeで範囲調整してからのほうが良い)
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->Fit("gaus");
}

void xrange(float xmin, float xmax){ // (オリジナル) 現在表示されているhistのx軸の表示範囲を設定
//	TObject *obj = gDirectory->GetListOfKeys()->FindObject(gPad->GetListOfPrimitives()->At(1)->GetName());
//	TH1 *h1 = (TH1*)gDirectory->Get(obj->GetName());
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->GetXaxis()->SetRangeUser(xmin,xmax);
	h1->Draw("same");
}

void xrange(){ // (オリジナル) 現在表示されているhistのx軸の表示範囲をデフォルトに戻す
//	TObject *obj = gDirectory->GetListOfKeys()->FindObject(gPad->GetListOfPrimitives()->At(1)->GetName());
//	TH1 *h1 = (TH1*)gDirectory->Get(obj->GetName());
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->GetXaxis()->UnZoom();
	h1->Draw("same");
}


void sly(){
	TH2D* h2 = (TH2D*)gPad->GetListOfPrimitives()->At(1);
	TString ptitle = h2->GetTitle();
	int binnum = h2->GetNbinsX();
	TH1D* firsth1;
	for(int i=0;i<binnum;i++){
		TH1D* h1 = h2->ProjectionY(Form("%s_sly%d",ptitle.Data(),i+1),i+1,i+1);
		float xstart = h2->GetXaxis()->GetBinLowEdge(i+1);
		float xend   = xstart + h2->GetXaxis()->GetBinWidth(i+1);
		h1->SetTitle(Form("%s_sly%d (x = %f : %f)",ptitle.Data(),i+1,xstart,xend));
		gDirectory->GetListOfKeys()->AddLast(h1);
		if(i==0) firsth1 = h1;
	}
        firsth1->Draw();
	TList* li = GetHistList();
        printf(" Draw ID:%3d  %s\n",li->IndexOf(firsth1),firsth1->GetName());
}





TList* GetHistList(){
	TList *li = new TList();
	if(gDirectory->GetListOfKeys() != nullptr){
		for(int i=0; i< gDirectory->GetListOfKeys()->GetEntries(); i++){
			li->Add(gROOT->FindObject(gDirectory->GetListOfKeys()->At(i)->GetName()));
		}
	}

	for(int i=0; i< gROOT->GetList()->GetEntries(); i++){
		li->Add(gROOT->GetList()->At(i));
	}
	return li;
}


// saho版からのコピー
float figali(float xmin, float xmax, 
		bool oldel =true, bool print = true, int kreturn =0, 
		TH1* ihist = 0x0 ){
	// oldel: old line delete
	// ihist 対象とするHist, 未入力or 0x0 指定で現在のヒストを取得する。
	
	TH1* h1;
	bool arghist = false; // 引数で処理するhistを指定してるか
	if(ihist == 0x0){
		h1 = ht();
	}else{
		h1 = ihist;
		//ihist->Draw();
		arghist = true;
	}

	// 以前この関数を使った際に描いたTGraphを消す
/*	if(gPad->GetListOfPrimitives()->FindObject("fli") != nullptr){
		gPad->GetListOfPrimitives()->FindObject("fli")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("fppl0") != nullptr){
		gPad->GetListOfPrimitives()->FindObject("fppl0")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("fppl1") != nullptr){
		gPad->GetListOfPrimitives()->FindObject("fppl1")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("fppl2") != nullptr){
		gPad->GetListOfPrimitives()->FindObject("fppl2")->Delete();
	}
*/

	float initialmin, initialmax;
	
	initialmin = h1->GetXaxis()->GetBinCenter(h1->GetXaxis()->GetFirst());
	initialmax = h1->GetXaxis()->GetBinCenter(h1->GetXaxis()->GetLast());
	h1->GetXaxis()->SetRangeUser(xmin,xmax);
	

	TF1 *fit1 = new TF1("fit1","[0]*exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))+[3]*x+[4]",xmin ,xmax);
	fit1->SetParameter(0,h1->GetBinContent(h1->GetMaximumBin()));
	fit1->SetParameter(1,h1->GetBinCenter(h1->GetMaximumBin()));
	fit1->SetParameter(2,(xmax - xmin)/10 );

	float tempx1 = xmin;
	float tempy1 = h1->GetBinContent(h1->FindBin(xmin));
	float tempx2 = xmax;
	float tempy2 = h1->GetBinContent(h1->FindBin(xmax));
	float tempa  = (tempy2-tempy1)/(tempx2-tempx1);
	float tempb  = tempy1 - tempa * tempx1;
	fit1->SetParameter(3,tempa);
	fit1->SetParameter(4,tempb);

	h1->Fit("fit1","RQ","");

	TGraph* gfit1 = new TGraph(fit1);
	gfit1->SetLineColor(2);
	gfit1->SetLineWidth(2);
	gfit1->Draw("same");

	TF1* fli = new TF1("fli",Form("%f*x+%f",fit1->GetParameter(3),fit1->GetParameter(4)),xmin ,xmax);

	TGraph* gli = new TGraph(fli);
	gli->SetLineColor(4);
	gli->SetLineWidth(1);
	if(!arghist){
		gli->Draw();
	}else{
		gli->Draw("same");
	}

	TGraph* ppl[3];
	float pplx[3];
	pplx[0] = fit1->GetParameter(1); // Mean
	pplx[1] = fit1->GetParameter(1) - 5 * TMath::Abs(fit1->GetParameter(2)); // Mean - 5 sigma
	pplx[2] = fit1->GetParameter(1) + 5 * TMath::Abs(fit1->GetParameter(2)); // Mean + 5 sigma
	for (int k=0; k<3; k++) {
		ppl[k] = new TGraph();
		ppl[k]->SetName(Form("fppl%d",k));
		ppl[k]->SetPoint(0, pplx[k], 0);
		ppl[k]->SetPoint(1, pplx[k], INFINITY);
		ppl[k]->SetLineColor(3);
		if(!arghist){ ppl[k]->Draw(); } else { ppl[k]->Draw("same"); }
	}
	

	int startbin = h1->FindBin(pplx[1]);
	int endbin   = h1->FindBin(pplx[2]);		
	float startval = h1->GetBinLowEdge(startbin);
	float endval   = h1->GetBinLowEdge(endbin+1);

//	printf("Constant : %f\n", fit1->GetParameter(0));

	float integral      = h1->Integral(startbin,endbin);
	float integralerr   = TMath::Sqrt(integral);
	float background    = fli->Integral(startval,endval)/0.25; // 要改善
	float backgrounderr = TMath::Sqrt(background);
	float calcedpeak    = integral - background;
	float calcedpeakerr = TMath::Sqrt(integral + background);

	if(print){
		printf("\n");
		printf("Center   : %#8g\n", fit1->GetParameter(1));
		printf("Sigma    : %#8g\n", TMath::Abs(fit1->GetParameter(2)));
		printf("Integral : %#8g  +-  %#8g\n", integral, integralerr );
		printf("CalcInte : %#8g\n", fit1->Integral(startval,endval)/0.25); // 要改善
		printf("B.G.     : %#8g  +-  %#8g\n", background, backgrounderr); 
		printf("Int - BG : %#8g  +-  %#8g\n", calcedpeak, calcedpeakerr); 

		printf("for me   : %#8g\t%#8g\t%#8g\n",fit1->GetParameter(1), calcedpeak, calcedpeakerr ); 
		printf("\n");
	}

	//xrange(initialmin, initialmax);
	if(!arghist){
		h1->GetXaxis()->SetRangeUser(initialmin, initialmax);
		h1->Draw("hist same");
	}
//gli->Draw();

	return fit1->GetParameter(1);
}

void xfigali(){
	printf("Click a Start and End point of fit range!\n");
	xgetvalx(2);
	figali(globalxvaldata.x[0], globalxvaldata.x[1]);
}

// 関数記述部

TH1* ht(int n, TString opt){ // n番目のhistをDraw (オプション指定)
	TH1* h1 = (TH1*)GetHistList()->At(n);
	h1->SetOption(opt);
	h1->Draw();
	return h1;
}



TH1* ht(){ // 現在表示されているhistのポインタを返すだけ ( ht()->Draw()的な使い方が出来るように )
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	return h1;
}

TH1* GetCurrentHist(){
	TH1* h1;
	if( gPad == 0x0 ){
		h1 = 0x0;
		fprintf(stderr, "Warning! No current TH1 !\n");
	}else{
		h1 = (TH1*)gPad->GetListOfPrimitives()->At(1);
	}
	return h1;
}

TH1* htp(int n=-1){ // Drawせずにポインタだけを返す
	TH1* h1;
	if(n<0){
		h1 = GetCurrentHist();
	}else{
		TList* li = GetHistList();
		h1 = (TH1*)li->At(n);
	}
	return h1;
}


TH1* ht(TString opt){ // 現在表示されているhistのoptionを変更して再Draw
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->SetOption(opt);
	h1->Draw();
	return h1;
}


TH1* hn(TString opt){ // 現在表示されているhistの次のhistを表示
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->After(gPad->GetListOfPrimitives()->At(1));
	h1->SetOption(opt);
	h1->Draw();
	printf(" Draw ID:%3d  %s\n",li->IndexOf(h1),h1->GetName());
	return h1;
}

TH1* hn(){ // ↑のオプション維持版
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->After(gPad->GetListOfPrimitives()->At(1));
	h1->Draw();
	printf(" Draw ID:%3d  %s\n",li->IndexOf(h1),h1->GetName());
	return h1;
}

TH1* hb(TString opt){ // 現在表示されているhistの前のhistを表示
	TH1* h1 = (TH1*)GetHistList()->Before(gPad->GetListOfPrimitives()->At(1));
	h1->SetOption(opt);
	h1->Draw();
	return h1;
}

TH1* hb(){ // ↑のオプション維持版
	TH1* h1 = (TH1*)GetHistList()->Before(gPad->GetListOfPrimitives()->At(1));
	h1->Draw();
	return h1;
}

void figa(){ // 現在表示されているhistをGausianでfit (xrangeで範囲調整してからのほうが良い)
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->Fit("gaus");
}

void xrange(float xmin, float xmax){ // (オリジナル) 現在表示されているhistのx軸の表示範囲を設定
//	TObject *obj = gDirectory->GetListOfKeys()->FindObject(gPad->GetListOfPrimitives()->At(1)->GetName());
//	TH1 *h1 = (TH1*)gDirectory->Get(obj->GetName());
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->GetXaxis()->SetRangeUser(xmin,xmax);
	h1->Draw("hist same");
}

void xrange(){ // (オリジナル) 現在表示されているhistのx軸の表示範囲をデフォルトに戻す
//	TObject *obj = gDirectory->GetListOfKeys()->FindObject(gPad->GetListOfPrimitives()->At(1)->GetName());
//	TH1 *h1 = (TH1*)gDirectory->Get(obj->GetName());
	TH1F *h1 = (TH1F*)gPad->GetListOfPrimitives()->At(1);
	h1->GetXaxis()->UnZoom();
	h1->Draw("same");
}





TList* GetHistList(){
	TList *li = new TList();
	if(gDirectory->GetListOfKeys() != nullptr){
		for(int i=0; i< gDirectory->GetListOfKeys()->GetEntries(); i++){
			li->Add(gROOT->FindObject(gDirectory->GetListOfKeys()->At(i)->GetName()));
		}
	}

	for(int i=0; i< gROOT->GetList()->GetEntries(); i++){
		li->Add(gROOT->GetList()->At(i));
	}
	return li;
}



