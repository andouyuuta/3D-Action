#pragma once
#include <DxLib.h>
#include <string>

class MoviePlayer
{
public:
    MoviePlayer(void);
    ~MoviePlayer(void);

    bool Load(const std::string& filePath, bool loop = false, int fadeFrame = 60);
    void Update(void);              // 再生・ループ・フェード処理
    void Draw(int x, int y);        // 描画
    void Stop(void);                // 手動停止

    bool IsPlaying(void) const;

private:
    int handle_;
    bool isLoop_;
    bool isPlaying_;
    int fadeFrame_;
    int currentFadeFrame_;
};
