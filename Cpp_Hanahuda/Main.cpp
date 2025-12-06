//"C:\Users\rk3b0\デスクトップ\github\Cpp_Hanahuda\Cpp_Hanahuda\App"
# include <Siv3D.hpp> // Siv3D v0.6.16
# include <memory>

#define ATehudaBasePosition {200,500}

struct HudaTextureManager
{
	static inline std::unique_ptr<Texture> textures[12][4];

	static void Load()
	{
		for (int m = 0; m < 12; ++m)
			for (int o = 0; o < 4; ++o)
				textures[m][o] = std::make_unique<Texture>(U"Images/{}_{}.jpg"_fmt(m + 1, o + 1));
	}

	static const Texture& Get(int m, int o)
	{
		if (!textures[m][o]) {
			// エンジン起動前やLoad未実行で呼ばれた場合は例外を投げる
			throw std::runtime_error("HudaTextureManager::Get: textures not loaded (call Load() after engine init).");
		}
		return *textures[m][o];
	}
};

struct Huda {
	int width;
	int height;
	int month;
	int order;
	Vec2 pos;

public: Huda() :month(0), order(0), width(40), height(60) {}
public: Huda(int m, int o) :month(m), order(o), width(40), height(60) {}
public:void SetBahuda(){
	this->width = 40;
	this->height = 60;
};
public:void SetMochihuda() {
	this->width = 40;
	this->height = 60;
};
public:void SetTehuda() {
	this->width = 80;
	this->height = 120;
};
public:void SetPosition(Vec2 position) {
	this->pos = position;
}
public:void Render() {
	HudaTextureManager::Get(month, order)
		.resized(width,height)
		.drawAt(pos);
}
};

using BahudaLines = std::array<Array<Huda>, 12>;
using TehudaLines = std::vector<Huda>;

void DrawRadialGradientBackground(const ColorF& centerColor, const ColorF& outerColor)
{
	Circle{ Scene::Center(), (Scene::Size().length() * 0.5) }
	.draw(centerColor, outerColor);
}

void DrawTable(int turn, BahudaLines& Bahuda, TehudaLines& ATehuda, TehudaLines& BTehuda) {
	Vec2 CenterPos = Scene::Center();
	Huda huda;
	//Render yamahuda
	RectF(Arg::center(CenterPos), huda.width, huda.height)
		.drawFrame(1, Palette::Black)
		.draw(ColorF(0.26, 0.43, 0.43));
	//Render bahuda
	huda.SetBahuda();
	int gap = 10;
	float BaseDx = gap * 1.5 + gap * 2 + huda.width * 3;
	float BaseDy = gap * 0.5 + huda.height * 0.5;
	//１～3
	Vec2 BasePoint = CenterPos + Vec2{ -BaseDx , -BaseDy };
	for (int i = 0; i < 3; i++) {
		Vec2 Position = BasePoint + i * Vec2(huda.width + gap, 0);
		auto MonthBahuda = Bahuda[i];
		for (auto& huda : MonthBahuda) {
			huda.SetPosition(Position);
			huda.Render();
		}
	}
	//４～６
	BasePoint = CenterPos + Vec2{ -BaseDx , +BaseDy };
	for (int i = 0; i < 3; i++) {
		Vec2 Position = BasePoint + i * Vec2(huda.width + gap, 0);
		auto MonthBahuda = Bahuda[i+3];
		for (auto& huda : MonthBahuda) {
			huda.SetPosition(Position);
			huda.Render();
		}
	}
	//７～９
	BasePoint = CenterPos + Vec2{ +gap * 1.5 + huda.width , -BaseDy };
	for (int i = 0; i < 3; i++) {
		Vec2 Position = BasePoint + i * Vec2(huda.width + gap, 0);
		auto MonthBahuda = Bahuda[i+6];
		for (auto& huda : MonthBahuda) {
			huda.SetPosition(Position);
			huda.Render();
		}
	}
	//１０～１２
	BasePoint = CenterPos + Vec2{ +gap * 1.5 + huda.width , +BaseDy };
	for (int i = 0; i < 3; i++) {
		Vec2 Position = BasePoint + i * Vec2(huda.width + gap, 0);
		auto MonthBahuda = Bahuda[i+9];
		for (auto& huda : MonthBahuda) {
			huda.SetPosition(Position);
			huda.Render();
		}
	}

	//Render Tehuda
	//A
	BasePoint = Vec2 ATehudaBasePosition;
	gap = -20;
	for (int i = 0; i < ATehuda.size();i++) {
		Huda& huda = ATehuda[i];
		Vec2 Position = BasePoint + i * Vec2(huda.width + gap, 0);
		huda.SetPosition(Position);
		huda.Render();
	}
	//B
	BasePoint = Vec2{ 200,100 };
	gap = -20;
	for (int i = 0; i < BTehuda.size(); i++) {
		Huda& huda = BTehuda[i];
		Vec2 Position = BasePoint + i * Vec2(huda.width + gap, 0);
		RectF(Arg::center(Position), huda.width, huda.height)
			.drawFrame(1, Palette::Black)
			.draw(ColorF(0.26, 0.43, 0.43));
	}
	return;
}

Huda GetNewHuda(bool (&BahudaAppeared)[12][4] ) {
	int NewHudaNum = Random(0, 47);
	int m, o;
	do
	{
		NewHudaNum = Random(0, 47);//0-47
		m = NewHudaNum / 4;
		o = NewHudaNum % 4;
	} while (BahudaAppeared[m][o]);
	BahudaAppeared[m][o] = true;
	Huda NewHuda = Huda(m, o);
	return NewHuda;
}

void InitializeTable(bool(&BahudaAppeared)[12][4],BahudaLines& Bahuda, TehudaLines& ATehuda, TehudaLines& BTehuda) {
	//setting bahuda
	int nonEmptyMonthCount = 0;
	while (nonEmptyMonthCount < 8)
	{
		Huda NewHuda = GetNewHuda(BahudaAppeared);
		NewHuda.SetBahuda();
		Bahuda[NewHuda.month].push_back(NewHuda);
		nonEmptyMonthCount = 0;
		for (int i = 0; i < 12; i++)
		{
			if (!Bahuda[i].isEmpty())
			{
				nonEmptyMonthCount++;
			}
		}
	}
	//setting tehuda
	for (int i = 0; i < 8; i++) {
		Huda NewHuda = GetNewHuda(BahudaAppeared);
		NewHuda.SetTehuda();
		ATehuda.push_back(NewHuda);
		NewHuda = GetNewHuda(BahudaAppeared);
		NewHuda.SetTehuda();
		BTehuda.push_back(NewHuda);
	}
}

//for Debugging
void PrintMonthsWithCards(const BahudaLines& bahuda)
{
	String line;
	bool first = true;

	for (int i = 0; i < 12; ++i)
	{
		if (!bahuda[i].isEmpty())
		{
			if (!first)
			{
				line += U", ";
			}
			line += Format(i + 1); // 月は 1〜12 で表示
			first = false;
		}
	}

	if (line.isEmpty())
	{
		line = U"(none)";
	}

	Print << U"Months with >=1 card: " << line;
}

//プレイヤーの手札クリック検知
//戻り値: クリックされた手札の index（0～7）or -1
int DetectSelectedTehuda(const TehudaLines& ATehuda,int& SelectedIndex, int& DecidedIndex)
{
	Vec2 BasePoint = Vec2{ 200, 500 };
	int gap = -20;

	int w = ATehuda[0].width;
	int h = ATehuda[0].height;

	for (int i = 0; i < ATehuda.size(); i++)
	{
		Vec2 pos = BasePoint + i * Vec2(w + gap, 0);

		// 画面上の当たり判定
		RectF hitbox(Arg::center(pos), w, h);

		if (hitbox.mouseOver())
		{
			SelectedIndex = i;
			// ダブルクリックで確定
			if (hitbox.leftClicked())
			{
				DecidedIndex = i;
			}
			break;
		}
	}
	return -1;
}

void HighlightCard(const Huda& card)
{
	RectF frame(
		Arg::center(card.pos),   // 描画位置と同一座標
		card.width + 8,
		card.height + 8
	);

	frame.drawFrame(5, Palette::Yellow);
}

void DrawYamahuda(BahudaLines& Bahuda, bool(&BahudaAppeared)[12][4]) {
	Huda NewHuda = GetNewHuda(BahudaAppeared);
	Bahuda[NewHuda.month].push_back(NewHuda);
}

void Main()
{
	//Print << FileSystem::CurrentDirectory();

	bool BahudaAppeared[12][4] = {};
	//A:Player B:Rival
	bool AMochihuda[12][4] = {};
	bool BMochihuda[12][4] = {};
	// 背景の色を設定する | Set the background color
	//Scene::SetBackground(ColorF{ 0.3, 0.9, 0.6 });
	int turn = 0;
	BahudaLines Bahuda;
	TehudaLines ATehuda, BTehuda;
	HudaTextureManager::Load();
	InitializeTable(BahudaAppeared,Bahuda, ATehuda, BTehuda);
	//for Debugging
	PrintMonthsWithCards(Bahuda);

	bool IsPlayerTurn = true;
	int SelectedIndex = -1;   
	int DecidedIndex = -1;  
	while (System::Update())
	{
		DrawRadialGradientBackground(ColorF{ 0.2, 0.8, 0.4 }, ColorF{ 0.26, 0.43, 0.35 });
		DrawTable(turn, Bahuda, ATehuda, BTehuda);

		if (ATehuda.empty() || BTehuda.empty()){
			return ;
		}
		if (IsPlayerTurn) {
			Huda DecidedHuda;
			int Clicked = DetectSelectedTehuda(ATehuda,SelectedIndex,DecidedIndex);
			//マウスオーバーされたものを強調表示
			if (SelectedIndex != -1)
			{
				HighlightCard(ATehuda[SelectedIndex]);
			}
			//ダブルクリックで決定
			if (DecidedIndex != -1)
			{
				DecidedHuda = ATehuda[DecidedIndex];
				if (!Bahuda[DecidedHuda.month].isEmpty()) {
					for (auto huda : Bahuda[DecidedHuda.month]) {
						//獲得
						AMochihuda[huda.month][huda.order] = true;
					}
					//場札から削除
					Bahuda[DecidedHuda.month].clear();
					//手札から削除
					ATehuda.erase(ATehuda.begin()+ DecidedIndex);
					SelectedIndex = -1;
					DecidedIndex = -1;
				}
				DrawYamahuda(Bahuda, BahudaAppeared);
			}

		}
		IsPlayerTurn = !IsPlayerTurn;
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
