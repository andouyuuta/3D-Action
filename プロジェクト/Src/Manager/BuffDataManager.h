#pragma once
#include <DxLib.h>
#include <string>
#include <vector>

// �o�t�̃f�[�^�Ǘ�
struct BuffData
{
    int id;                     // ���ʔԍ�
    std::string imagePath_;     // �摜�p�X
    int AppearRate;             // �o���m��
    int attackUp_;              // �U���͏㏸��
    int hpUp_;                  // HP�㏸��
    int successRate_;           // �����m��(��)
    int valueSuccess_;          // �������̒l
    int valueFailure_;          // ���s���̒l
    std::string description_;   // ����

    // CSV�t�@�C���ɂ͏����Ă��Ȃ����
    int handleId_ = -1;         // �摜�n���h��
    int graph_Width = 0;
    int graph_Height = 0;
};

class BuffDataManager
{
public:
    static BuffDataManager& GetInstance();

    BuffDataManager(void);

    void Release(void);

    void LoadFromCSV(const std::string& path);              // CSV�t�@�C������o�t�f�[�^��ǂݍ���
    const BuffData& GetBuffData(int id)const;               // �w��ID�ɑΉ�����o�t�f�[�^��Ԃ�
    const BuffData& GetBuffDataByIndex(int index)const;     // �z��Ńo�t�f�[�^���擾
    int GetBuffCount()const;                                // �o�^����Ă���f�[�^��Ԃ�
private:
    static BuffDataManager instance_;
    std::vector<BuffData> buffList_;
};

