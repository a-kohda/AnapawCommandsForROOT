/**
* @file
* @brief ANAPAW Commands for ROOT で使える関数の定義
* @author A. Kohda
* @date 2023. 11. 9
* @version 1.11
*/

// グローバル変数の定義
TString defaultdrawopt = "colz"; 
vector<double> gDoubleVec;

// ANAPAW準拠のユーザー用関数
// 基本操作用の関数
/**
* ROOTファイルに含まれるヒストグラム等の一覧表示。行頭にヒストグラムIDも表示。
*/
void hlist();                // Index番号付きでヒストグラムのリストを表示する

/**
* hlistへのエイリアス。ROOTファイルに含まれるヒストグラムの一覧表示
*/
void (*ls)() = hlist;        // "hlist()"は"ls()"でも可
//void ht(int n, TString opt); // n番目のhistをDraw (オプション指定あり)
//void ht(TString opt);        // 現在表示されているhistのoptionを変更して再Draw
//void hn(TString opt);        // 現在表示されているhistの次のhistを表示
//void hb(TString opt);        // 現在表示されているhistの前のhistを表示

// フィッティング系
//void fig();                  // 現在表示されているhistをGausianでfit (現在表示中の全範囲で)
//void xfitg();                // マウスで最大、最小を指定してGausianでfit
//void fitgl(float xmin, float xmax); // gaus+直線でfit
//void xfitgl();
//void fiterf(float xmin, float xmax); // 誤差関数でfit

// 表示調整系
void lgx();                  /*!< x軸をログスケールにする   */
void lnx();                  /*!< x軸をリニアスケールにする */
void lgy();                  /*!< y軸をログスケールにする   */
void lny();                  /*!< y軸をリニアスケールにする */
void lgz();                  /*!< z軸をログスケールにする   */
void lnz();                  /*!< z軸をリニアスケールにする */

// 2次元 -> 1次元
//void sly(int n = -1);        // 2次元ヒストグラムをスライスしてy軸への射影

// オリジナルのユーザー用関数
//TH1*  htp(int n = -1);        // HID指定、または現在表示されているhistのポインタを返す


// このマクロ内で使用する関数
/// @cond
TH1* GetCurrentHist(bool quiet = false);// 現在表示されているhistのポインタを返す
int  GetObjID(TObject* o1);  // ヒスト等のオブジェクトがリストの何番目にあるかを返す
TList* GetHistList();        // .lsで表示されるオブジェクトのリスト
void DrawHist(TH1* h1, TString opt = defaultdrawopt);
//void DeleteObjFromGPad(TString name); // gPad上にある(TGraphなどの)オブジェクトを消す(nameで指定)
void SetAPStyle();           // Histの見栄えをANAPAWっぽいstyleにする
void CdNPad();
/// @endcond

// 関数エイリアス
/**
* 現在表示中のヒストグラムのポインタを取得する
*/
//TH1* gH1(){ return GetCurrentHist(true); }
TH1* gH1;
void SetgH1(){
	gH1 = GetCurrentHist(true);
}



///////// 関数・クラスの実体記述部 //////////

/// @cond
TH1* GetCurrentHist(bool quiet){
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
/// @endcond

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

/**
* HIDを数値指定して、そのヒストグラムを表示(Draw)する。
* @param n 表示するヒストグラムID
* @param opt Draw Option
*/
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

/// @cond
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
			if(currentHID < li->GetEntries()-1 ){ currentHID ++; }
			else{ currentHID = -1; }
		}
		if(kind==2){
			if(currentHID > 0 ){ currentHID --; /*printf("test\n");*/}
			else{ currentHID = linum; }
		}
	}
	if(exdraw){ DrawHist(h1_2, opt); }
}
/// @endcond

void hn(TString opt = defaultdrawopt){ S_hNorB(1, opt); }
void hb(TString opt = defaultdrawopt){ S_hNorB(2, opt); }


/// @cond
void DrawHist(TH1* h1, TString opt){
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
	//h1->SetXTitle(Form("Histogram ID = %d",GetObjID(h1)));
	h1->Draw(opt);

	// 情報表示
	TText *tfilename = new TText(0.01,0.01,gDirectory->GetName());
	tfilename->SetNDC(1);
	tfilename->SetTextAlign(11);
	tfilename->Draw();
	TText *t_hid = new TText(0.90,0.05,Form("Histogram ID = %d",GetObjID(h1)));
	t_hid->SetNDC(1);
	t_hid->SetTextAlign(32);
	t_hid->Draw();

	TDatime *currenttime = new TDatime();
	TText *tdatetime = new TText(0.99,0.99,currenttime->AsSQLString() );
	tdatetime->SetNDC(1);
	tdatetime->SetTextAlign(33);
	tdatetime->Draw();

	printf(" Draw ID:%3d  %s\n",GetObjID(h1),h1->GetTitle());
  defaultdrawopt = opt;

	gH1 = h1;
}
/// @endcond

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

void chbin(int ngroupx, int ngroupy=1, int ngroupz=1){ // 現在x,yどちらかのみ
	TH1D* h1 = (TH1D*)GetCurrentHist();
	if( h1==0x0 ) { return; }
	TString ptitle = h1->GetTitle(); 
	TString pname  = h1->GetName();
	TH1D* h1_copied = (TH1D*)h1->Clone();
	if( ngroupy==1 ){ // xのみ
		h1_copied->SetTitle(Form("%s (chbx%d)",ptitle.Data(),ngroupx));
		h1_copied->SetName(Form("%s_chbx%d",pname.Data(),ngroupx));
		h1_copied->Rebin(ngroupx);
	}
	if( ngroupx==1 ){ // yのみ
		h1_copied->SetTitle(Form("%s (chby%d)",ptitle.Data(),ngroupy));
		h1_copied->SetName(Form("%s_chby%d",pname.Data(),ngroupy));
		((TH2D*)h1_copied)->RebinY(ngroupy);
	}
	

	gDirectory->GetListOfKeys()->AddLast(h1_copied);
	DrawHist(h1_copied);
	gPad->Modified();
	gPad->Update();
	gPad->Modified();
}

void blowx(float xmin, float xmax){ // ANAROOT準拠。旧バージョンのxrange。
	TH1 *h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	h1->GetXaxis()->SetRangeUser(xmin,xmax);
	gPad->Modified();
	gPad->Update();
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

/*! 単なるblowはblowxの代用(暫定) */
void blow(float xmin=0, float xmax=-1){
	if(xmin > xmax){
		blowx();
	}else{
		blowx(xmin,xmax);
	}
}


void mami(float min = 1, float max = -1){ 
	TH2 *h1 = (TH2*)GetCurrentHist();
	if(h1 == 0x0) return;
	if(h1->InheritsFrom("TH2")){
		if(min < max){
			h1->GetZaxis()->SetRangeUser(min,max);
		}else{
			h1->GetZaxis()->UnZoom();
		}
	}else{
		if(min < max){
			blowy(min,max);
		}else{
			blowy();
		}
	}

	gPad->Modified();
}

/// @cond
void S_prXorY(int kind, float min = 0, float max = -1){ // 開発中
	if(kind != 1 && kind != 2) return; // 1:x, 2:y
	
	TH2 *h2 = (TH2*)GetCurrentHist();
	if(h2 == 0x0) return;
	TH1D* h1;

	if(min>max){
		if(kind==1) h1 = h2->ProjectionX("_p");
		if(kind==2) h1 = h2->ProjectionY("_p");
	}else{
		int binmin, binmax;
		if(kind==1){
			binmin = h2->GetYaxis()->FindBin(min); 
			binmax = h2->GetYaxis()->FindBin(max);
		}
		if(kind==2){
			binmin = h2->GetXaxis()->FindBin(min);
			binmax = h2->GetXaxis()->FindBin(max);
		}
		if(kind==1) h1 = h2->ProjectionX("_p", binmin, binmax);
		if(kind==2) h1 = h2->ProjectionY("_p", binmin, binmax);
	}
	if(kind==1) h1->SetTitle(Form("%s px%#6g-%#6g",h2->GetTitle(),min,max));
	if(kind==2) h1->SetTitle(Form("%s py%#6g-%#6g",h2->GetTitle(),min,max));

	TString pname  = h2->GetName();
	while(true){
		if(kind==1 && gROOT->FindObject(Form("%s_px",pname.Data()))==0x0 ) break;
		if(kind==2 && gROOT->FindObject(Form("%s_py",pname.Data()))==0x0 ) break;
		pname += "_"; // nameが重複する場合"_"をもう一個足す。
	}

	if(kind==1) h1->SetName(Form("%s_px",pname.Data()));
	if(kind==2) h1->SetName(Form("%s_py",pname.Data()));
	gDirectory->GetListOfKeys()->AddLast(h1);
	DrawHist(h1);
}
/// @endcond

void prx(float min = 0, float max = -1){ S_prXorY(1, min, max); }
void pry(float min = 0, float max = -1){ S_prXorY(2, min, max); }


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
	gPad->Modified();
}

void print(TString ext="png"){
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
			TString fname = Form("%s/root%d_%d.%s",
				sdircand[i].Data(),
				TDatime().GetDate(),TDatime().GetTime(),ext.Data());

			//printf("%s\n",fname.Data());
			gPad->GetCanvas()->SaveAs(fname.Data());
			saved = true;
			break;
		}
	}
	if(!saved) printf(" Not saved because candidate directories not found.\n");
}

/*! キャンバスの表示内容を画像化してメール送信 */
/*! 環境変数 APCR_MAILTO に送信先メールアドレスを設定 */
void mail(){
	char *mailto;
	mailto = getenv("APCR_MAILTO");
	if(mailto==NULL){
		printf("Error : Environmental Variable APCR_MAILTO is not setting!\n");
		printf("You can set your mail adress by using :\n");
		printf("   > setenv(\"APCR_MAILTO\",\"your@mail.adress.jp\",1)\n");
		return;
	}
	//printf("test:%s\n",mailto);
	if(gPad == 0x0) return;
	TString tmpfilename = Form("/tmp/roottmpimg/img%08x.png",rand());
	system("mkdir -p /tmp/roottmpimg");
	int tempErrorIgnoreLevel = gErrorIgnoreLevel;
	gErrorIgnoreLevel = 2000; // save as 時に Infoを出さない
	gPad->GetCanvas()->SaveAs(tmpfilename.Data());
	gErrorIgnoreLevel = tempErrorIgnoreLevel; // 戻しておく
	system(Form("echo \"\"|mail -s \"ROOT\" -a %s %s",tmpfilename.Data(), mailto));
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

/// @cond
void CdNPad(){
	if(gPad == 0x0) return;
//	if(gPad == gPad->GetCanvas()) return; // 分割されてなければ終了
	if(gPad->GetCanvas()->GetPad(1) == 0x0 ) return; // PadNo1 がなければ終了
	int currentPadNum = gPad->GetNumber();
	if(gPad->GetCanvas()->GetPad(currentPadNum+1)==0x0 ){
		gPad->GetCanvas()->cd(1); // 次のPadが0x0ならPadNo1に移動
	}else{
		gPad->GetCanvas()->cd(currentPadNum+1);
	}
//	if(gPad == gPad->GetCanvas()->cd(gPad->GetNumber()+1) ){
//		gPad->GetCanvas()->cd(1);	
//	}
}
/// @endcond


void (*cdn)() = CdNPad;        // 
// cd, cdbもそのうち作る


void size(float w, float h){ // デフォルトのサイズに対する比率で指定
	if(gPad == 0x0) return;
	int defw = 600;
	int defh = 600;
	int difw = 2;
	int difh = 24;
	gPad->GetCanvas()->SetWindowSize (w * defw + difw, h * defh + difh);
}

void figali(float xmin, float xmax, 
		bool oldel =true, bool print = true, int kreturn =0, 
		TH1* ihist = 0x0 ){
	// oldel: old line delete
	// ihist 対象とするHist, 未入力or 0x0 指定で現在のヒストを取得する。
	
	TH1* h1;
	bool arghist = false; // 引数で処理するhistを指定してるか
	if(ihist == 0x0){
		h1 = GetCurrentHist();
	}else{
		h1 = ihist;
		//ihist->Draw();
		arghist = true;
	}

	// 以前この関数を使った際に描いたTGraphを消す
	if(gPad->GetListOfPrimitives()->FindObject("fli") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("fli")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("fppl0") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("fppl0")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("fppl1") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("fppl1")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("fppl2") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("fppl2")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("gfit1") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("gfit1")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("ptfitinfob") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("ptfitinfob")->Delete();
	}
	if(gPad->GetListOfPrimitives()->FindObject("ptfitinfot") != 0x0){
		gPad->GetListOfPrimitives()->FindObject("ptfitinfot")->Delete();
	}



	float initialmin, initialmax;
	
	initialmin = h1->GetXaxis()->GetBinCenter(h1->GetXaxis()->GetFirst());
	initialmax = h1->GetXaxis()->GetBinCenter(h1->GetXaxis()->GetLast());
	h1->GetXaxis()->SetRangeUser(xmin,xmax);
	

	TF1 *fit1 = new TF1("fit1","[0]*exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))+[3]*x+[4]",xmin ,xmax);
	fit1->SetParName(0,"Constant");
	fit1->SetParName(1,"Mean");
	fit1->SetParName(2,"Sigma");
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

	h1->Fit("fit1","RQN","");

	TGraph* gfit1 = new TGraph(fit1);
	gfit1->SetName("gfit1");
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
	float background    = fli->Integral(startval,endval)/5.; // 要改善
	float backgrounderr = TMath::Sqrt(background);
	float calcedpeak    = integral - background;
	float calcedpeakerr = TMath::Sqrt(integral + background);

	if(print){
		printf("\n");
		printf("Center   : %#8g\n", fit1->GetParameter(1));
		printf("Sigma    : %#8g\n", TMath::Abs(fit1->GetParameter(2)));
		printf("Integral : %#8g  +-  %#8g\n", integral, integralerr );
		printf("CalcInte : %#8g\n", fit1->Integral(startval,endval)/5.); // 要改善
		printf("B.G.     : %#8g  +-  %#8g\n", background, backgrounderr); 
		printf("Int - BG : %#8g  +-  %#8g\n", calcedpeak, calcedpeakerr); 

	//	printf("for me   : %#8g\t%#8g\t%#8g\n",fit1->GetParameter(1), calcedpeak, calcedpeakerr ); 
		printf("\n");
		printf("Paramaters\n");
		printf("%#6g\t %#6g\t %#6g\t %#6g\t %#6g\n",
			fit1->GetParameter(0), fit1->GetParameter(1), fit1->GetParameter(2), fit1->GetParameter(3), fit1->GetParameter(4));
		
	}

	//xrange(initialmin, initialmax);
	if(!arghist){
		h1->GetXaxis()->SetRangeUser(initialmin, initialmax);
		//h1->Draw("hist same");
		gPad->Modified();
		gPad->Update();
		gPad->Modified();
	}
//gli->Draw();

	//return fit1->GetParameter(1);
	delstat();
	TPaveText *pt1 = new TPaveText(.4,.7,.9,.9,"NDC");
	TPaveText *pt2 = new TPaveText(.55,.7,.9,.9,"NDC");
	pt1->SetName("ptfitinfob");
	pt2->SetName("ptfitinfot");
	pt1->SetTextSize (gStyle->GetStatFontSize());
	pt2->SetTextSize (gStyle->GetStatFontSize());
	pt1->SetTextAlign(12);
	pt2->SetTextAlign(12);
	pt1->SetBorderSize(1);
	pt2->SetBorderSize(0);
	pt1->SetFillStyle(0);
	pt2->SetFillStyle(0);
	pt1->AddText("Center");
		pt2->AddText(Form("%#8g", fit1->GetParameter(1)));
	pt1->AddText("Sigma");
		pt2->AddText(Form("%#8g", TMath::Abs(fit1->GetParameter(2))));
	pt1->AddText("Integral");
		pt2->AddText(Form("%#8g #pm %#8g", integral, integralerr));
	pt1->AddText("CalcInte");
		pt2->AddText(Form("%#8g", fit1->Integral(startval,endval)/5.)); // 要改善
	pt1->AddText("B.G.");
		pt2->AddText(Form("%#8g #pm %#8g", background, backgrounderr));
	pt1->AddText("Int - BG");
		pt2->AddText(Form("%#8g #pm %#8g", calcedpeak, calcedpeakerr));
	pt1->Draw();
	pt2->Draw();
}

// copy from https://root-forum.cern.ch/t/how-to-fit-a-skew-gaussian/50922
//double skewedgauss(double * x, double *p) {
 //   double xi = p[0];
//    double omega = p[1];
//    double alpha = p[2];
//    double arg = (x[0] - xi) / omega;
//    double smallphi = TMath::Gaus(arg, 0.0, 1.0, true);
//    double bigphi = 0.5 * (1 + std::erf(alpha * arg/std::sqrt(2)));
//    return 2./omega * smallphi * bigphi;
//}
//void Set_skewedgauss();
//void Set_skewedgauss(){
//   auto f1 = new TF1("skewgaus",skewedgauss,0,20,3);
//   f1->SetParameters(2,1);
   //f1->SetParNames("constant","coefficient");
   //f1->Draw();
//}
//Set_skewedgauss();

/*! Fit with Skew Gaussian + Linear function */
void fitsgl(float xmin, float xmax ){
	TH1* h1 = (TH1*)GetCurrentHist();
	if(h1 == 0x0) return;
	float binwidth = h1->GetBinWidth(1); // ビン幅一定でないと正しく計算できないので注意
	int   startbin = h1->FindBin(xmin);
	int   endbin   = h1->FindBin(xmax);		

	float x0 = h1->GetBinLowEdge(startbin); // binの開始位置に微調整
	float x1 = h1->GetBinLowEdge(endbin+1);
	float y0 = h1->GetBinContent(startbin);
	float y1 = h1->GetBinContent(endbin);

	// 初期値
	Double_t pinitial[6];
	pinitial[0] = (x0+x1)/2; // Shift
	pinitial[1] = (x1-x0)/4; // Spread
	pinitial[2] = 0;         // Skewness
	pinitial[3] = h1->GetBinContent((int)(startbin+endbin)/2);// Amplitude 
	pinitial[4] = (y1-y0)/(x1-x0); // Slope 
	pinitial[5] = (y0+y1)/2;   // Offset

	// Fitting
	TF1 *fitfunc = new TF1("f_fit",Form("[3]*TMath::Gaus((x -[0])/[1],0.0,1.0,true)*0.5*(1+erf([2]*(x-[0])/[1]/sqrt(2)))+[4]*(x-%f)+[5]",(x1+x0)/2),x0 ,x1);
	fitfunc->SetParNames("p0:Shift","p1:Spread","p2:Skewnes","p3:Amplitu","p4:Slope","p5:Offset");
	fitfunc->SetParameters(pinitial);
	h1->Fit("f_fit","","",x0,x1);
	Double_t *presult =  fitfunc->GetParameters(); // presult[3]みたいな感じでアクセスできる
	const Double_t *perrs   =  fitfunc->GetParErrors (); 
	//printf("%f\n",presult[0]);

	// 解析
//	TF1 *f_bg =  new TF1("f_bg",Form("[0]*(x-%f)+[1]",(x1+x0)/2),x0 ,x1);
//	f_bg->SetParameters(presult[4],presult[5]);


	//float background  = f_bg->Integral(x0,x1)/binwidth; 
	float background2 = presult[5]*(x1-x0)/binwidth; 
	float funcinte = fitfunc->Integral(x0,x1)/binwidth; 
	float histinte      = h1->Integral(startbin,endbin);

	//float backgrounderr = TMath::Sqrt(background);
	float backgrounderr2= perrs[5]*(x1-x0)/binwidth;
	float histinteerr = TMath::Sqrt(histinte);

	float calcpeak    = histinte - background2;
	float calcpeakerr = TMath::Sqrt(histinte + backgrounderr2*backgrounderr2);

	//float backgrounderrtest =0;// f_bg->IntegralError(startval,endval)/binwidth; 

	//printf("background : %f +- %f (%f)\n",background, backgrounderr,backgrounderrtest);
	printf("Background : %.1f +- %.1f\n",background2, backgrounderr2);
	printf("FuncInte   : %.1f\n",funcinte);
	printf("HistInte   : %.1f +- %.1f (%.1f-%.1f)\n",histinte,histinteerr,histinte-histinteerr,histinte+histinteerr);
	printf("Peak       : %.1f +- %.1f\n",calcpeak,calcpeakerr);


	hupdate();
}


void hdump(){
	TH1* h1 = (TH1*)GetCurrentHist();
  if(h1 == 0x0) return;



	char *home_dir;
	home_dir = getenv("HOME");
	//printf("test:%s\n",mailto);


	// 保存場所
	vector<TString> sdircand; // 保存ディレクトリの候補
	sdircand.push_back("./png");    // 候補順に書く
	sdircand.push_back("../png"); 
	sdircand.push_back("./plots"); 
	sdircand.push_back("../plots");
	sdircand.push_back(home_dir);
	//sdircand.push_back("./");

	bool find_flag = false;
	int sdir_num = -1;
	for(int i=0;i<sdircand.size();i++){
		char command[128];
		sprintf(command, "test -d %s", sdircand[i].Data()); // OS依存性あり
		//printf("%s\n",command);
		int returnval = system(command);
		//printf("%d\n",returnval);
		if(returnval == 0){
			sdir_num = i;
			break;
		}
	}
	if(sdir_num < 0) {
		printf(" Not saved because candidate directories not found.\n");
		return;
	}



	TString fname = Form("%s/dump.dat",
		sdircand[sdir_num].Data());
	//printf("%s\n",fname.Data());
	
	FILE *fout = fopen(fname.Data(), "w");

	if( h1->InheritsFrom("TH2") ){ // TH2の場合
		TH2* h2 = (TH2*)h1;
		int fNbinsX = h2->GetNbinsX();
		int fNbinsY = h2->GetNbinsY();
		for(int biny=0; biny<fNbinsY+2; biny++){
		for(int binx=0; binx<fNbinsX+2; binx++){
			float x = h2->GetXaxis()->GetBinCenter(binx);
			float y = h2->GetYaxis()->GetBinCenter(biny);
			float z = h2->GetBinContent(binx,biny);
			fprintf(fout,"%f %f %f\n",x,y,z);
		}}
		fclose(fout);
		printf("Dumped at plots/hdump.txt\n");
		return;
	}

	int nbin = h1->GetNbinsX();
	for(int n=0;n<nbin+2;n++){
		float x = h1->GetBinCenter(n);
		float y = h1->GetBinContent(n);
		fprintf(fout,"%f %f\n",x,y);
	}	
	fclose(fout);
	printf("Dumped at %s/dump.dat\n",sdircand[sdir_num].Data());
}

void hdump2D(){
	FILE *fout = fopen("hdump2D.tdm", "w");
	
	TH1* h1 = (TH2*)GetCurrentHist();
  if(h1 == 0x0) return;

	int minbx = h1->GetXaxis()->FindBin(2.40) -1;
	int maxbx = h1->GetXaxis()->FindBin(2.70) +1;
	int minby = h1->GetYaxis()->FindBin(18.5) -1;
	int maxby = h1->GetYaxis()->FindBin(24.5) +1;

	int nxbin = h1->GetXaxis()->GetNbins();
	int nybin = h1->GetYaxis()->GetNbins();
	printf("Info : (nx,xy)=(%d,%d)",maxbx-minbx+1,maxby-minby+1);

	fprintf(fout,"READ MESH\nFOR X=");
	for(int nx=0;nx<nxbin+2;nx++){
		if(nx<minbx||maxbx<nx) continue;
		float x = h1->GetXaxis()->GetBinCenter(nx);
		fprintf(fout,"%#5g ",x);
	}
	fprintf(fout,"\n");
	for(int ny=0;ny<nybin+2;ny++){
		if(ny<minby||maxby<ny) continue;
		float y = h1->GetYaxis()->GetBinCenter(ny);		
		fprintf(fout,"Y=%#5g Z=",y);
		for(int nx=0;nx<nxbin+2;nx++){
			if(nx<minbx||maxbx<nx) continue;
			float x = h1->GetXaxis()->GetBinCenter(nx);
			float z = h1->GetBinContent(nx,ny);
			//fprintf(fout,"%f %f %f\n",x,y,z);
			//fprintf(fout,"%#5g ",z);
			if(z<1)z=1;
			//fprintf(fout,"%#5g ",log10(z));
			//fprintf(fout,"%#5g ",sqrt(z));
			fprintf(fout,"%#5g ",z);
			
		}
		fprintf(fout,"\n");
	}	
	fclose(fout);
	printf("Dumped at hdump2D.tdm\n");
}


/*! TH1をTF1に変換 */
void htofunc(TString fname = "hfunc1");
void htofunc(TString fname){
	//TString fname = "hfunc1"; 
	FILE *fout = fopen(Form("f%s.c",fname.Data()), "w");
	
	fprintf(fout,"double f%s(double x, double p0){\n",fname.Data());
	fprintf(fout,"\tdouble f=0;\n");

	TH1* h1 = (TH1*)GetCurrentHist();
  if(h1 == 0x0) return;
	int nbin = h1->GetNbinsX();
	for(int n=0;n<nbin;n++){
		float x0 = h1->GetBinLowEdge(n+1);
		float x1 = h1->GetBinLowEdge(n+2);
		float y = h1->GetBinContent(n+1);
		fprintf(fout,"\tif( %f<=x && x<%f ) f = %f * p0;\n",x0,x1,y);
	}	
	fprintf(fout,"\treturn f;\n");
	fprintf(fout,"}\n");
	fclose(fout);
	printf("Created at f%s.c\n",fname.Data());
	printf("When you use this function for fit, type;\n");
	printf("\t> .L f%s.c\n",fname.Data());
	printf("\t> TF1* tf%s = new TF1(\"tf%s\",\"f%s(x,[0])\",0,4000);\n",fname.Data(),fname.Data(),fname.Data());
}

/*! Tree->Draw()で作ったhistを、ls で表示されるリストに追加する */
void adl(){
  TH1* h1 = (TH1*)GetCurrentHist();
  TH1D* h1_copied = (TH1D*)h1->Clone();
  h1_copied->SetName(Form("h%08x\n",rand()));
  gDirectory->GetListOfKeys()->AddLast(h1_copied);
	DrawHist(h1_copied);
	hupdate();
}

/*! histogramの情報を出力 */
void info(){
	TH1* h1 = (TH1*)GetCurrentHist();
	printf("Title  : %s\n",h1->GetTitle());
	printf("Name   : %s\n",h1->GetName());
	printf("Entries: %lf\n",h1->GetEntries());
	printf("Mean   : %lf\n",h1->GetMean());
	printf("Std Dev: %lf\n",h1->GetStdDev());

}

/*! 現在開かれているファイル一覧 */
void fls(){
	int num = gROOT->GetListOfFiles()->Capacity();
	for(int i=0; i<num; i++){
		printf("    %2d  %s\n",i,gROOT->GetListOfFiles()->At(i)->GetName() );
	}
}

/*! histgram同士の割り算 */
void divide(int hid1, int hid2){
	TList* li = GetHistList();
	TH1* h1 = (TH1*)li->At(hid1);
	if(h1 == 0x0 || ! h1->InheritsFrom("TH1") ){
		printf(" %d do not exist or not histgram\n", hid1);
		return;
	}
	TH1* h2 = (TH1*)li->At(hid2);
	if(h2 == 0x0 || ! h1->InheritsFrom("TH1") ){
		printf(" %d do not exist or not histgram\n", hid2);
		return;
	}
	TH1* h1_copied = (TH1*)h1->Clone();
	h1_copied->Divide(h2);

	h1_copied->SetName(Form("h%08x\n",rand()));
	h1_copied->SetTitle(Form("h%d / h%d",hid1, hid2) );
	gDirectory->GetListOfKeys()->AddLast(h1_copied);
	DrawHist(h1_copied);
	hupdate();
}

/// @cond
void switchAPmode(){
	TString apmodepath = "/home/kohda/.rootmacros/AnapawCommandsForROOT/apmode";
	TString openingfile = gDirectory->GetName();
	if( openingfile.EqualTo("Rint") ) openingfile = "";
	gROOT->ProcessLine(Form(".! %s %s",apmodepath.Data(),openingfile.Data()));
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
		gStyle->SetTextSize(fsize);
		//gStyle->SetLegendTextSize(fsize);
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
	gStyle->SetTitleY(0.95);
}

// 以下は自分用のstyle設定

void ReDrawSlideStyle(){
	gPad->GetCanvas()->SetWindowSize (800 + 2, 600+ 24);
	int linewidth =3;
	gPad->SetFrameLineWidth(linewidth);
	gStyle->SetLineWidth(linewidth);
	gPad->SetMargin(0.11,0.04,0.08,0.06);
	TH1* h1 = (TH1*)GetCurrentHist();	
	h1->Draw();
	h1->SetTitle(";;;");
	h1->SetStats(0);
	int fontid=63;
	float fsize = 28; // フォントサイズ(px)
	h1->SetLabelFont(fontid,"XYZ");
	h1->SetLabelFont(fontid,"");
	h1->SetLabelSize(fsize,"XYZ");
	h1->SetLabelSize(fsize,"");
	h1->SetLabelOffset (0.03,"XY");
	gPad->SetGrid(0,0);
	gPad->SetTicks();
	h1->GetXaxis()->SetNdivisions(7);
	h1->GetYaxis()->SetNdivisions(7);
	h1->SetLineWidth(linewidth);
	h1->SetFillStyle(0);
	h1->SetLineColor(0);
	h1->SetLineColor(1);
	gPad->Modified();
	gPad->Update();
	gPad->Modified();
}

void ReDrawPaperStyle2Djpg(){
	int linewidth =0;
	gPad->SetFrameLineWidth(linewidth);
	gStyle->SetLineWidth(linewidth);
	gPad->SetMargin(0.16,0.08,0.16,0.08); // lrbt
	//gStyle->SetPalette(kGreyScale);
	//TColor::InvertPalette();
  const Int_t NRGBs = 2;   
  const Int_t NCont = 128;  
  Double_t stops[NRGBs] = { 0.00, 1.00 }; 
  Double_t Red[NRGBs]   = { 1.00, 0.10 }; 
  Double_t Green[NRGBs] = { 1.00, 0.10 }; 
  Double_t Blue[NRGBs]  = { 1.00, 0.10 }; 
  TColor::CreateGradientColorTable(NRGBs, stops, Red, Green, Blue, NCont);
  gStyle->SetNumberContours(NCont);

	TH1* h1 = (TH1*)GetCurrentHist();	
	h1->Draw("col");
	h1->SetTitle(";;;");
	h1->SetStats(0);
	h1->SetLabelOffset (100,"XY");
	gPad->SetGrid(0,0);
	h1->SetLineWidth(linewidth);
	h1->SetFillStyle(0);
	gPad->Modified();
	gPad->Update();
	gPad->Modified();
}

void ReDrawPaperStyle2Dframe(){
	TH1* h1 = (TH1*)GetCurrentHist();	
	float minX = h1->GetXaxis()->GetXmin();
	float maxX = h1->GetXaxis()->GetXmax();
	float minY = h1->GetYaxis()->GetXmin();
	float maxY = h1->GetYaxis()->GetXmax();

	gPad->Clear();
	gStyle->SetTitleStyle(0);
	gPad->SetFillStyle(0);
	TH1F *frame = gPad->DrawFrame( minX, minY, maxX, maxY);
	frame->SetFillStyle(0);
	gStyle->SetLineWidth(1);
	gPad->SetGrid(0,0);
	gPad->SetTicks();
	frame->GetXaxis()->SetNdivisions(7);
	frame->GetYaxis()->SetNdivisions(7);
	gPad->GetFrame()->SetFillStyle(0);


}
/// @endcond

