// ANAPAW Commands for ROOT
// Version 1.0
// Last Updated 2022.11.16 by A. Kohda 


// sahoにある版と、ubuntu16にある版を統合する
// root5/6の切り替えを不要にする
// sahoにあるeucalib.cxxのGetXYbyClickを導入する
// 全体的に可読性を良くする

// デフォルトの変数設定
TString defaultdrawopt = "colz"; 

// ANAPAW準拠のユーザー用関数
void hlist();                // Index番号付きでヒストグラムのリストを表示する
void (*ls)() = hlist;        // "hlist()"は"ls()"でも可
void ht(int n, TString opt = defaultdrawopt); // n番目のhistをDraw (オプション指定あり)
void ht(TString opt);        // 現在表示されているhistのoptionを変更して再Draw
void hn(TString opt = defaultdrawopt);  // 現在表示されているhistの次のhistを表示
void hb(TString opt = defaultdrawopt);  // 現在表示されているhistの前のhistを表示
void fig();                  // 現在表示されているhistをGausianでfit (現在表示中の全範囲で)
void lgy();                  // y軸をログスケールにする
void lny();                  // y軸をリニアスケールにする
void sly(int n = -1);        // 2次元ヒストグラムをスライスしてy軸への射影

// オリジナルのユーザー用関数
TH1*  htp(int n = -1);        // 引数指定、または現在表示されているhistのポインタを返す
void  xrange(float xmin, float xmax);
void  xrange();
float figali(float xmin, float xmax, bool oldel =true, bool print = true, int kreturn =0, TH1* ihist = 0x0 );
void  xfigali();


// このマクロ内で使用する関数
TH1* GetCurrentHist();       // 現在表示されているhistのポインタを返す
int  GetObjID(TObject* o1);  // ヒスト等のオブジェクトがリストの何番目にあるかを返す
TList* GetHistList();
class GetXYbyClick {         // マウスを使って点の取得を行う
	private:
		int maxnum; // 取得する最大数
		TString name;
		vector<float> x;
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
vector<double> globalvecdouble;
GetXYbyClick   globalxyclick("globalxyclick");


