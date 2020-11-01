#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <algorithm>
#include <dirent.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <main_app/texture/Texture.h>
#include <main_app/timer_mgr/TimerMgr.hpp>

enum Shaders
{
    eShaderTexture,
    eShaderWave,
    eShaderClip,
    eShaderCrop,
    eShaderLens,
    eCountOfShaders
};

enum Effects
{
    eEffectInactive,
    eEffectBlade,
    eCountOfEffects
};

struct TextureDataDebug
{
    TextureDataDebug(const std::string& strPath = "");
    std::shared_ptr<Texture> ptrTexture;
    float fX = 0.0f;
    float fY = 0.0f;
    int nDynamicSourceX = 0;
    int nDynamicSourceY = 0;
    int nDynamicSourceW;
    int nDynamicSourceH;
    bool bSelected = false;
    float fAngle = 0.0f;
    float fScaleFactor = 1.0f;
    unsigned int unShaderID = 0;
};

struct AnimDataDebug
{
    std::vector<std::shared_ptr<Texture>>vecAnimation;
    std::string sPath = "";
    bool bSelected = false;
    float fX = 0.0f;
    float fY = 0.0f;
    float fAngle = 0.0f;
    float fScaleFactor = 1.0f;
    unsigned int unShaderID = 0;
    int nFPS = 25;
    int nCurrentFrame = 0;
    TimerMgr mTimerAnim;
};

class Debug
{

private:

    static constexpr unsigned int m_unBuffSymbols = 1024;
    std::shared_ptr<Texture> textureStreak;
    std::shared_ptr<Texture> textureX;

    Shader shaderRect;
    Shader shaderTexture;
    Shader shaderTexture3D;
    Shader shaderWave;
    Shader shaderClip;
    Shader shaderCrop;
    Shader shaderLens;

    glm::vec4 m_vec4ClipCrop;
    glm::vec4 m_vec4EffectDatas;

    bool m_bInfoPopup;
    bool m_bWriteIntoFile;
    bool m_bModifyPathWriteIntoFile;
    bool m_bShowImguiHelp;
    bool m_bCreateTexture;
    bool m_bCreateAnimation;
    bool m_bSceneOptions;
    bool m_bReplaceCurrentTexture;
    bool m_bEnableDrag;
    bool m_bEnableCut;
    bool m_bSimulateRotation;
    bool m_bShaders;
    bool m_bEnable3D;
    bool m_bAnimAlreadyLoaded;
    bool m_bRotateAroundAxisX;
    bool m_bRotateAroundAxisY;
    bool m_bRotateAroundAxisZ;
    bool m_bShowPivot;

    ImVec4 m_vec4Color;
    std::vector<Shader>m_vecShaders;
    std::vector<TextureDataDebug>m_vecTextureData;
    std::vector<AnimDataDebug>m_vecAnimData;
    std::vector<std::string>m_vecStrFiles;

    char m_cArrSymbols[m_unBuffSymbols];
    char m_cArrSymbolsReload[m_unBuffSymbols];
    char m_cArrSymbolsAnim[m_unBuffSymbols];
    char m_cArrSymbolsWritePathAndFile[m_unBuffSymbols];

    int m_nCurrShader;
    int m_nCurrentEffect;

    float m_fSpeedRotation;
    float m_fMemoryUsagePNG;
    float m_fMemoryUsageDDS;
    float m_fAlphaPivot;

    TimerMgr m_timerRotation;
    TimerMgr m_timerShowInfo;
    TimerMgr m_timerShowPivot;

    void MainWindow();
    void TextureAndRectModify();
    void RadioButtonTextures();
    void CreateTextureAndTextureOptions();
    void CreateAnimAndAnimOpitions();
    void SceneMenu();
    void WriteIntoFile();
    void EnableDisableDrag();
    void SimulateRotation();
    void SimulateRotationLabel();
    void CalculateMemoryOfPNGDDS();
    void EnableDisableShader();
    void EnableDisableImageCut();
    void RightClickAndSelect();
    void TimerProcess();
    void Draw();
    void CurrentShader(int nShaderToApply);
    void CurrentEffect(int nEffectToApply);
public:

    Debug();
    ~Debug();
    bool Init(const std::string& strXMLSavePath);
    void Process();
    void Deinit();
    void NewFrame();
    bool DragFromOutside(int nPathCount, const char* strPaths[]);
    inline std::vector<TextureDataDebug>& GetVecTextureData() {return m_vecTextureData;}
    inline std::vector<AnimDataDebug>& GetVecAnimData() {return m_vecAnimData;}
    inline bool& GetRotateAroundX() {return m_bRotateAroundAxisX;}
    inline bool& GetRotateAroundY() {return m_bRotateAroundAxisY;}
    inline bool& GetRotateAroundZ() {return m_bRotateAroundAxisZ;}
    inline bool& GetEnable3D() {return m_bEnable3D;}

    float g_fXCoord;
    float g_fYCoord;
    float g_fZCoord;
    float g_fXCoordMemory;
    float g_fYCoordMemory;

    float g_fAngle;
    float g_fHorizontalAxisPivot;
    float g_fVerticalAxisPivot;
    float g_fScale;
};
#endif
