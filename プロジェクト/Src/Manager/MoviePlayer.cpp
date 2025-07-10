#include "MoviePlayer.h"

MoviePlayer::MoviePlayer(void)
    : handle_(-1), isLoop_(false), isPlaying_(false),
    fadeFrame_(0), currentFadeFrame_(0)
{
}

MoviePlayer::~MoviePlayer(void)
{
    if (handle_ != -1)
    {
        DeleteGraph(handle_);
    }
}

bool MoviePlayer::Load(const std::string& filePath, bool loop, int fadeFrame)
{
    if (handle_ != -1)
    {
        DeleteGraph(handle_);
    }

    handle_ = LoadGraph(filePath.c_str());
    if (handle_ == -1)
        return false;

    PlayMovieToGraph(handle_);
    isLoop_ = loop;
    fadeFrame_ = fadeFrame;
    currentFadeFrame_ = 0;
    isPlaying_ = true;
    return true;
}

void MoviePlayer::Update(void)
{
    if (!isPlaying_ || handle_ == -1)
        return;

    if (GetMovieStateToGraph(handle_) == TRUE)
    {
        if (isLoop_)
        {
            SeekMovieToGraph(handle_, 0);
            PlayMovieToGraph(handle_);
            currentFadeFrame_ = 0; // フェード再開
        }
        else
        {
            isPlaying_ = false;
        }
    }
    else
    {
        if (currentFadeFrame_ < fadeFrame_)
            ++currentFadeFrame_;
    }
}

void MoviePlayer::Draw(int x, int y)
{
    if (!isPlaying_ || handle_ == -1)
        return;

    int alpha = 255;
    if (fadeFrame_ > 0 && currentFadeFrame_ < fadeFrame_)
    {
        alpha = (255 * currentFadeFrame_) / fadeFrame_;
    }

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    DrawGraph(x, y, handle_, FALSE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void MoviePlayer::Stop(void)
{
    if (handle_ != -1)
    {
        PauseMovieToGraph(handle_);
        isPlaying_ = false;
    }
}

bool MoviePlayer::IsPlaying(void) const
{
    return isPlaying_;
}
