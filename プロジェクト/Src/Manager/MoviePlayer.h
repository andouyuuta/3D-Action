#pragma once
#include <DxLib.h>
#include <string>

class MoviePlayer
{
public:
    MoviePlayer(void);
    ~MoviePlayer(void);

    bool Load(const std::string& filePath, bool loop = false, int fadeFrame = 60);
    void Update(void);              // �Đ��E���[�v�E�t�F�[�h����
    void Draw(int x, int y);        // �`��
    void Stop(void);                // �蓮��~

    bool IsPlaying(void) const;

private:
    int handle_;
    bool isLoop_;
    bool isPlaying_;
    int fadeFrame_;
    int currentFadeFrame_;
};
