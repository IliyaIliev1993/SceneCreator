#include "Debug.h"
#include <main_app/renderer/Renderer.h>
#include <main_app/debug/Utils.h>
#include <main_app/debug/Log.h>
#include <main_app/MainApp.h>

const unsigned int TIMER_ID_ANIMATION = 1;
const unsigned int TIMER_ID_SIMULATE_ROTATION = 2;
const unsigned int TIMER_ID_SHOW_INFO = 3;
const unsigned int TIMER_ID_PIVOT = 4;

TextureDataDebug::TextureDataDebug(const std::string& strPath)
{
    ptrTexture = Texture::CreateTexture(strPath);
    nDynamicSourceW = ptrTexture->GetWidth();
    nDynamicSourceH = ptrTexture->GetHeight();
}

Debug::Debug()
    : shaderRect()
    , shaderTexture()
    , shaderTexture3D()
    , shaderWave()
    , shaderClip()
    , shaderCrop()
    , shaderLens()
    , m_vec4ClipCrop{0.0f, 0.0f, 1.0f, 1.0f}
    , m_vec4EffectDatas{1.0f, 1.0f, 1.0f, 1.0f}
    , textureStreak(nullptr)
    , textureX(nullptr)
    , m_bInfoPopup(false)
    , m_bWriteIntoFile(false)
    , m_bModifyPathWriteIntoFile(false)
    , m_bShowImguiHelp(false)
    , m_bCreateTexture(false)
    , m_bCreateAnimation(false)
    , m_bSceneOptions(false)
    , m_bReplaceCurrentTexture(false)
    , m_bEnableDrag(false)
    , m_bEnableCut(false)
    , m_bSimulateRotation(false)
    , m_bShaders(false)
    , m_bEnable3D(false)
    , m_bShowPivot(false)
    , m_vec4Color(ImVec4(1.00f, 1.00f, 1.00f, 1.00f))
    , m_cArrSymbols{"/home/default/"}
    , m_cArrSymbolsReload{"/home/default/"}
    , m_cArrSymbolsAnim{"/home/default/Animation/scatter_anim/ EXAMPLE"}
    , m_cArrSymbolsWritePathAndFile{"/home/default/Desktop/ExportFromSceneCreator.xml"}
    , m_nCurrShader(eShaderTexture)
    , m_nCurrentEffect(eEffectInactive)
    , m_bAnimAlreadyLoaded(false)
    , m_bRotateAroundAxisX(false)
    , m_bRotateAroundAxisY(false)
    , m_bRotateAroundAxisZ(true)
    , m_fSpeedRotation(0.1f)
    , m_fMemoryUsagePNG(0.0f)
    , m_fMemoryUsageDDS(0.0f)
    , m_fAlphaPivot(0.0f)
    , m_timerRotation()
    , m_timerShowInfo()
    , m_timerShowPivot()
    , g_fXCoord(0.0f)
    , g_fYCoord(0.0f)
    , g_fZCoord(0.0f)
    , g_fXCoordMemory(0.0f)
    , g_fAngle(0.0f)
    , g_fHorizontalAxisPivot(0.0f)
    , g_fVerticalAxisPivot(0.0f)
    , g_fScale(0.0f)

{
}

Debug::~Debug()
{
}

bool Debug::Init(const std::string& strXMLSavePath)
{
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(MainApp::Get().ptrRenderer->GetWindow(), true);
    ImGui::StyleColorsDark();
    ImGui::GetStyle().FrameRounding = 5.0f;
    ImGui::GetStyle().GrabRounding = 5.0f;

    shaderRect = Shader("../src/shaders/vertex_rect.vs", "../src/shaders/fragment_rect.fs");
    shaderTexture = Shader("../src/shaders/vertex_2D.vs", "../src/shaders/fragment_2D.fs");
    shaderTexture3D = Shader("../src/shaders/vertex_3D.vs", "../src/shaders/fragment_3D.fs");
    shaderWave = Shader("../src/shaders/vertex_wave.vs", "../src/shaders/fragment_wave.fs");
    shaderClip = Shader("../src/shaders/vertex_clip.vs", "../src/shaders/fragment_clip.fs");
    shaderCrop = Shader("../src/shaders/vertex_crop.vs", "../src/shaders/fragment_crop.fs");
    shaderLens = Shader("../src/shaders/vertex_lens.vs", "../src/shaders/fragment_lens.fs");

    m_vecShaders = {shaderTexture, shaderWave, shaderClip, shaderCrop, shaderLens};

    textureStreak = Texture::CreateTexture("../src/resources/streak.png");
    if(!textureStreak->Load())
    {
        return false;
    }

    textureX = Texture::CreateTexture("../src/resources/x.png");
    if(!textureX->Load())
    {
        return false;
    }

    g_fZCoord = MainApp::Get().ptrRenderer->SCREEN_DEPTH;

    std::strcpy(m_cArrSymbolsWritePathAndFile, strXMLSavePath.c_str());

    return true;
}

bool Debug::DragFromOutside(int nPathCount, const char *strPaths[])
{
    std::string sPath = *strPaths;
    if(m_bCreateAnimation)
    {
        sPath.append("/");
        std::strcpy(m_cArrSymbolsAnim, sPath.c_str());

        return true;
    }

    if(m_bReplaceCurrentTexture)
    {
        std::strcpy(m_cArrSymbolsReload, sPath.c_str());
        return true;
    }

    for(int nCurrPath = 0; nCurrPath < nPathCount; nCurrPath++)
    {
        TextureDataDebug dataToPush(strPaths[nCurrPath]);
        if(dataToPush.ptrTexture->Load())
        {
            m_vecTextureData.emplace_back(dataToPush);
        }
    }

    return true;
}

void Debug::RadioButtonTextures()
{
    ImGui::Text("");
    if(!m_vecTextureData.empty())
    {
        ImGui::Text("Select a Texture to work with");
    }

    for(unsigned int i = 0; i < m_vecTextureData.size(); i++)
    {
        std::string strPath = m_vecTextureData.at(i).ptrTexture->GetPath() +
                " - " +
                std::to_string(i) +
                " - " +
                "X: " +
                std::to_string((int)m_vecTextureData.at(i).fX) +
                " - " +
                "Y: " +
                std::to_string((int)m_vecTextureData.at(i).fY);

        if(ImGui::RadioButton(strPath.c_str(),
                              m_vecTextureData.at(i).bSelected))
        {
            g_fXCoord = m_vecTextureData.at(i).fX;
            g_fYCoord = m_vecTextureData.at(i).fY;
            g_fXCoordMemory = m_vecTextureData.at(i).fX;
            g_fYCoordMemory = m_vecTextureData.at(i).fY;
            g_fAngle = m_vecTextureData.at(i).fAngle;
            g_fScale = m_vecTextureData.at(i).fScaleFactor;
            m_vecTextureData.at(i).bSelected = !m_vecTextureData.at(i).bSelected;
            m_vecTextureData.at(i).nDynamicSourceX = m_vecTextureData.at(i).ptrTexture->GetSourceX();
            m_vecTextureData.at(i).nDynamicSourceY = m_vecTextureData.at(i).ptrTexture->GetSourceY();
            m_vecTextureData.at(i).nDynamicSourceW = m_vecTextureData.at(i).ptrTexture->GetWidth();
            m_vecTextureData.at(i).nDynamicSourceH = m_vecTextureData.at(i).ptrTexture->GetHeight();
        }

        if(m_vecTextureData.at(i).bSelected)
        {
            m_vecTextureData.at(i).fX = g_fXCoord;
            m_vecTextureData.at(i).fY = g_fYCoord;
            m_vecTextureData.at(i).fAngle = g_fAngle;
            m_vecTextureData.at(i).fScaleFactor = g_fScale;
        }

    }

    ImGui::Text("");
    if(!m_vecAnimData.empty())
    {
        ImGui::Text("Select Animation to work with");

        for(unsigned int i = 0; i < m_vecAnimData.size(); i++)
        {
            std::string sPathAnim = m_vecAnimData.at(i).sPath;
            std::string strPath = sPathAnim +
                    " - " +
                    std::to_string(i) +
                    " - " +
                    "X: " +
                    std::to_string((int)m_vecAnimData.at(i).fX) +
                    " - " +
                    "Y: " +
                    std::to_string((int)m_vecAnimData.at(i).fY);

            if(ImGui::RadioButton(strPath.c_str(),
                                  m_vecAnimData.at(i).bSelected))
            {
                g_fXCoord = m_vecAnimData.at(i).fX;
                g_fYCoord = m_vecAnimData.at(i).fY;
                g_fXCoordMemory = m_vecAnimData.at(i).fX;
                g_fYCoordMemory = m_vecAnimData.at(i).fY;
                g_fAngle = m_vecAnimData.at(i).fAngle;
                g_fScale = m_vecAnimData.at(i).fScaleFactor;
                m_vecAnimData.at(i).bSelected = !m_vecAnimData.at(i).bSelected;
            }

            if(m_vecAnimData.at(i).bSelected)
            {
                m_vecAnimData.at(i).fX = g_fXCoord;
                m_vecAnimData.at(i).fY = g_fYCoord;
                m_vecAnimData.at(i).fAngle = g_fAngle;
                m_vecAnimData.at(i).fScaleFactor = g_fScale;
            }
        }

    }

}

void Debug::CreateTextureAndTextureOptions()
{
    if (m_bCreateTexture)
    {
        ImGui::Begin("Texture Options", &m_bCreateTexture);
        ImGui::Text("Texture Path:");

        ImGui::InputText("",m_cArrSymbols,m_unBuffSymbols);
        ImGui::SameLine();

        if (ImGui::Button("Add new texture"))
        {
            TextureDataDebug mData(m_cArrSymbols);
            if(mData.ptrTexture->Load())
            {
                m_vecTextureData.emplace_back(mData);
            }
        }

        ImGui::Text("");
        ImGui::Text("Texture Loaded: %zu", m_vecTextureData.size());
        ImGui::Text("");

        //Tree Textures
        for(unsigned int i = 0; i < m_vecTextureData.size(); i++)
        {
            auto& currentData = m_vecTextureData.at(i);
            std::string strNodeId = currentData.ptrTexture->GetPath() + " - " + std::to_string(i);
            if(ImGui::TreeNode(strNodeId.c_str()))
            {
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.7f, 0.0f, 0.0f, 1.0f});
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.0f, 0.0f, 0.0f, 1.0f});
                if (ImGui::Button("Remove texture"))
                {
                    /* Clear the current texture */
                    m_vecTextureData.erase(m_vecTextureData.begin() + i);
                }
                ImGui::PopStyleColor(2);

                ImGui::SameLine();
                if (ImGui::Button("Replace current texture"))
                {
                    m_bReplaceCurrentTexture = true;
                }
                else if(m_bReplaceCurrentTexture)
                {
                    ImGui::Begin("Replace current texture", &m_bReplaceCurrentTexture);
                    ImGui::Text("Insert new path or drag HERE the texture to replace");
                    ImGui::InputText("",m_cArrSymbolsReload,m_unBuffSymbols);
                    if(std::strcmp(m_cArrSymbolsReload, m_vecTextureData.at(i).ptrTexture->GetPath().c_str()) == 0)
                    {
                        if(ImGui::Button("Same Path!"))
                        {
                            m_bReplaceCurrentTexture = false;
                        }

                        ImGui::End();
                    }
                    else
                    {
                        if (ImGui::Button("Replace"))
                        {
                            currentData.ptrTexture = Texture::CreateTexture(m_cArrSymbolsReload);
                            currentData.ptrTexture->Load();
                            m_bReplaceCurrentTexture = false;
                        }

                        ImGui::End();
                    }
                }

                ImGui::TreePop();
            }
        }



        if (ImGui::Button("Close"))
        {
            m_bCreateTexture = false;
        }

        ImGui::End();
    }

}

void Debug::CreateAnimAndAnimOpitions()
{
    if(m_bCreateAnimation)
    {
        ImGui::Begin("Animation Options", &m_bCreateAnimation);
        ImGui::Text("Insert Animation path or Drag Here the folder:");
        ImGui::InputText("",m_cArrSymbolsAnim,m_unBuffSymbols);
        ImGui::SameLine();

        ImGui::BeginGroup();
        if (ImGui::Button("Create ID's and Load Textures"))
        {
            m_vecStrFiles.clear();
            std::string sPathToAnim = m_cArrSymbolsAnim;
            Utils::GetDirectoryFiles(sPathToAnim,m_vecStrFiles);

            LOG_TRACE("sPathToAnim: nCurrPath -> \"{0}\" has {1} files.\n",
                      sPathToAnim, m_vecStrFiles.size());

            if(!m_vecStrFiles.empty())
            {
                AnimDataDebug mDataToPush;
                mDataToPush.sPath = sPathToAnim;
                for(unsigned int i = 0; i < m_vecStrFiles.size(); i++)
                {
                    std::string strPath = m_vecStrFiles.at(i);
                    std::shared_ptr<Texture> textureToPush = Texture::CreateTexture(strPath);

                    if(textureToPush->Load())
                    {
                        mDataToPush.vecAnimation.emplace_back(textureToPush);
                    }
                }
                m_vecAnimData.emplace_back(mDataToPush);
            }
        }
        ImGui::EndGroup();
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Check the Loading process in console");
        }

        ImGui::NewLine();
        ImGui::Text("Loaded Animations: %zu", m_vecAnimData.size());
        ImGui::NewLine();

        for(unsigned int i = 0; i < m_vecAnimData.size(); i++)
        {
            std::string sNodeName = m_vecAnimData.at(i).sPath + " - " + std::to_string(i);
            if(ImGui::TreeNode(sNodeName.c_str()))
            {
                if(!m_vecAnimData.empty())
                {
                    if (ImGui::Button("Play"))
                    {
                        if(!m_vecAnimData.at(i).mTimerAnim.IsStarted())
                        {
                            m_vecAnimData.at(i).mTimerAnim.Start(i, (int)(1000 / m_vecAnimData.at(i).nFPS));
                        }

                    }

                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    ImGui::SliderInt("Frame", &m_vecAnimData.at(i).nCurrentFrame, 0, m_vecAnimData.at(i).vecAnimation.size() -1);
                    ImGui::EndGroup();
                    if(m_bEnableDrag && ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Drag Enabled !");
                    }

                    if (ImGui::Button("Pause"))
                    {
                        m_vecAnimData.at(i).mTimerAnim.Stop();
                    }
                    if (ImGui::Button("Stop"))
                    {
                        m_vecAnimData.at(i).mTimerAnim.Stop();
                        m_vecAnimData.at(i).nCurrentFrame = 0;
                    }

                    const std::string sFrameBegin = m_vecAnimData.at(i).vecAnimation.at(0)->GetPath().c_str();
                    const std::string sFrameEnd = m_vecAnimData.at(i).vecAnimation.at( m_vecAnimData.at(i).vecAnimation.size()-1 )->GetPath().c_str();
                    const unsigned int unFramesCount = m_vecAnimData.at(i).vecAnimation.size();

                    ImGui::Text("Frame Begin: %s", sFrameBegin.c_str());
                    ImGui::Text("Frame End: %s", sFrameEnd.c_str());
                    ImGui::Text("Frames Count: %u", unFramesCount);

                    ImGui::InputInt("FPS", &m_vecAnimData.at(i).nFPS);
                    ImGui::SameLine();
                    if(ImGui::Button("Change FPS"))
                    {
                        if(m_vecAnimData.at(i).nFPS <= 0)
                        {
                            m_vecAnimData.at(i).nFPS = 1;
                        }
                        else if(m_vecAnimData.at(i).nFPS >= 60)
                        {
                            m_vecAnimData.at(i).nFPS = 60;
                        }
                        m_vecAnimData.at(i).mTimerAnim.Stop();
                        m_vecAnimData.at(i).mTimerAnim.Start(i, (int)(1000 / m_vecAnimData.at(i).nFPS));
                    }

                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.7f, 0.0f, 0.0f, 1.0f});
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.0f, 0.0f, 0.0f, 1.0f});
                    if(ImGui::Button("Remove Animation"))
                    {
                        m_vecAnimData.at(i).mTimerAnim.Stop();
                        m_vecAnimData.at(i).nCurrentFrame = 0;
                        m_vecAnimData.erase(m_vecAnimData.begin() + i);
                    }
                    ImGui::PopStyleColor(2);
                }

                ImGui::NewLine();
                ImGui::TreePop();
            }
        }

        if (ImGui::Button("Close"))
        {
            m_bCreateAnimation = false;
        }

        ImGui::End();
    }
}

void Debug::SceneMenu()
{
    if(!m_bSceneOptions)
    {
        return;
    }

    ImGui::Begin("Scene Options", &m_bSceneOptions, ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("New Scene"))
            {

            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }



    ImGui::End();
}

void Debug::WriteIntoFile()
{
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.7f, 0.0f, 0.0f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.0f, 0.0f, 0.0f, 1.0f});
    if (ImGui::Button("Write changes into file..."))
    {
        std::ofstream dataToWrite(m_cArrSymbolsWritePathAndFile);

        std::string sData = "<game_rss_list>\n"
                            "<ResourceExpression expression = \"{  }\" />\n";

        for(unsigned int i = 0; i < m_vecTextureData.size(); i++)
        {

            sData += "\t<Image key=\"IMAGE_ID_" + std::to_string(i + 1) + "\"" +
                     " fileName=\"" + m_vecTextureData.at(i).ptrTexture->GetPath() + "\"" +
                     " x=" + "\"" + std::to_string(m_vecTextureData.at(i).ptrTexture->GetSourceX()) + "\"" +
                     " y=" + "\"" + std::to_string(m_vecTextureData.at(i).ptrTexture->GetSourceY()) + "\"" +
                     " width=" + "\"" + std::to_string(m_vecTextureData.at(i).ptrTexture->GetWidth()) + "\"" +
                     " height=" + "\"" + std::to_string(m_vecTextureData.at(i).ptrTexture->GetHeight()) + "\"" +
                     " priority=\"200\"/>\n";

        }
        sData += "</game_rss_list>\n";
        sData += "\n";
        sData += "DESTINATIONS IMAGES: \n";

        for(unsigned int i = 0; i < m_vecTextureData.size(); i++)
        {
            sData +="IMAGE_ID_" + std::to_string(i + 1) + " - " +
                    "X = " +
                    std::to_string((int)m_vecTextureData.at(i).fX) +
                    " Y = " +
                    std::to_string((int)m_vecTextureData.at(i).fY) + " \n";
        }

        sData += "\n";
        sData += "DESTINATIONS ANIMATIONS: \n";

        for(unsigned int i = 0; i < m_vecAnimData.size(); i++)
        {
            sData +="ANIM_PATH = " + m_vecAnimData.at(i).sPath + " - " +
                    "X = " +
                    std::to_string((int)m_vecAnimData.at(i).fX) +
                    " Y = " +
                    std::to_string((int)m_vecAnimData.at(i).fY) +
                    " FPS = " +
                    std::to_string(m_vecAnimData.at(i).nFPS) + " \n";
        }

        dataToWrite << sData;
        dataToWrite.close();

        ImGui::OpenPopup("Info");

    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    ImGui::Checkbox("Path Options", &m_bModifyPathWriteIntoFile);

    if(m_bModifyPathWriteIntoFile)
    {
        ImGui::Begin("Path and file", &m_bModifyPathWriteIntoFile);
        ImGui::InputText("Path and File",m_cArrSymbolsWritePathAndFile,m_unBuffSymbols);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.7f, 0.0f, 0.0f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.0f, 0.0f, 0.0f, 1.0f});
        if(ImGui::Button("Apply..."))
        {
            m_bModifyPathWriteIntoFile = false;
        }
        ImGui::PopStyleColor(2);
        ImGui::End();
    }


    if (ImGui::BeginPopupModal("Info", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::NewLine();
        ImGui::Text("File written: %s", m_cArrSymbolsWritePathAndFile);
        ImGui::NewLine();
        if (ImGui::Button("OK", ImVec2(500,0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

}

void Debug::EnableDisableShader()
{
    ImGui::Checkbox("Shaders,Effects", &m_bShaders);

    if(!m_bShaders)
    {
        return;
    }

    ImGui::Begin("Shaders and Effects", &m_bShaders);

    const char* strArrListOfShaders[eCountOfShaders] = {"NONE", "Wave", "Clip", "Crop", "Lens"};
    ImGui::Text("Shaders");
    ImGui::ListBox("List of Shaders", &m_nCurrShader, strArrListOfShaders, eCountOfShaders);

    switch (m_nCurrShader)
    {
    case eShaderWave:
    {
        if(ImGui::Button("Reset Values"))
        {
            m_vec4ClipCrop.x = 0.0f;
            m_vec4ClipCrop.y = 0.0f;
            m_vec4ClipCrop.w = 1.0f;
            m_vec4ClipCrop.z = 5.0f;
        }
        ImGui::InputFloat("Center wave X", &m_vec4ClipCrop.x, 0.01f);
        ImGui::InputFloat("Center wave Y", &m_vec4ClipCrop.y, 0.01f);
        ImGui::InputFloat("Aplitude", &m_vec4ClipCrop.w, 0.01f);
        ImGui::InputFloat("Speed", &m_vec4ClipCrop.z, 0.01f);
    }
        break;
    case eShaderClip:
    case eShaderCrop:
    {
        if(ImGui::Button("Reset Values"))
        {
            m_vec4ClipCrop.x = 0.0f;
            m_vec4ClipCrop.y = 0.0f;
            m_vec4ClipCrop.w = 50.0f;
            m_vec4ClipCrop.z = 50.0f;
        }
        ImGui::InputFloat("X", &m_vec4ClipCrop.x, 1.0f);
        ImGui::InputFloat("Y", &m_vec4ClipCrop.y, 1.0f);
        ImGui::InputFloat("W", &m_vec4ClipCrop.w, 1.0f);
        ImGui::InputFloat("H", &m_vec4ClipCrop.z, 1.0f);
    }
        break;

    case eShaderLens:
    {
        if(ImGui::Button("Reset Values"))
        {
            m_vec4ClipCrop.x = 0.0f;
            m_vec4ClipCrop.y = 0.0f;
            m_vec4ClipCrop.w = 0.5f;
        }
        ImGui::InputFloat("X", &m_vec4ClipCrop.x, 0.005f);
        ImGui::InputFloat("Y", &m_vec4ClipCrop.y, 0.005f);
        ImGui::InputFloat("Zoom", &m_vec4ClipCrop.w, 0.005f);
    }
        break;

    default:
        break;
    }

    if(ImGui::Button("Apply Shader..."))
    {
        for(unsigned int i = 0; i < m_vecTextureData.size(); i++)
        {
            if(m_vecTextureData.at(i).bSelected)
            {
                m_vecTextureData.at(i).unShaderID = m_nCurrShader;
            }
        }
    }

    ImGui::NewLine();
    const char* strArrListOfEffects[eCountOfEffects] = {"NONE", "BladeEffect"};
    static int nCurrentEffect = 0;

    ImGui::Text("Effects");
    ImGui::ListBox("List of Effects", &nCurrentEffect, strArrListOfEffects, eCountOfEffects);

    switch (nCurrentEffect)
    {

    case eEffectBlade:
    {
        if(ImGui::Button("Reset Values"))
        {
            m_vec4EffectDatas.x = 2.0f;
            m_vec4EffectDatas.y = 15.0f;
            m_vec4EffectDatas.z = 8.0f;
            m_vec4EffectDatas.w = 1.0f;
        }
        ImGui::InputFloat("Space between objects", &m_vec4EffectDatas.x, 1.0f);
        ImGui::InputFloat("Count of objects", &m_vec4EffectDatas.y, 1.0f);
        ImGui::InputFloat("Thickness", &m_vec4EffectDatas.z, 1.0f);
        ImGui::InputFloat("Speed of fade", &m_vec4EffectDatas.w, 1.0f);

        m_vec4EffectDatas.x = std::max(m_vec4EffectDatas.x, 0.0f);
        m_vec4EffectDatas.y = std::max(m_vec4EffectDatas.y, 1.0f);
        m_vec4EffectDatas.z = std::max(m_vec4EffectDatas.z, 0.0f);
        m_vec4EffectDatas.w = std::max(m_vec4EffectDatas.w, 0.0f);
    }
        break;

    default:
        break;
    }

    if(ImGui::Button("Apply Effect..."))
    {
        m_nCurrentEffect = nCurrentEffect;
    }

    ImGui::End();

}

void Debug::CurrentEffect(int nEffectToApply)
{
    switch (nEffectToApply)
    {
    case eEffectBlade:
    {
        static glm::vec4 vec4DataOfObject = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        static std::vector<glm::vec4>vecDatas;
        static float fAlpha = 1.0f;
        unsigned int unSpaceBetweenObjects = m_vec4EffectDatas.x;
        unsigned int unCountOfObjects = m_vec4EffectDatas.y;
        float fThickness = m_vec4EffectDatas.z;
        unsigned int unSpeedOfFade = m_vec4EffectDatas.w;
        if(ImGui::IsMouseClicked(0))
        {
            vec4DataOfObject.x = (float)ImGui::GetMousePos().x;
            vec4DataOfObject.y = (float)ImGui::GetMousePos().y;
            fAlpha = 1.0f;
            m_timerShowInfo.Stop();
        }

        if(ImGui::IsMouseDown(0))
        {
            vec4DataOfObject.z = vec4DataOfObject.x + ImGui::GetMouseDragDelta(0).x;
            vec4DataOfObject.w = vec4DataOfObject.y + ImGui::GetMouseDragDelta(0).y;
            vecDatas.push_back(vec4DataOfObject);
        }

        if(fAlpha != 0.0f)
        {
            MainApp::Get().ptrRenderer->SetColor(1.0f, 1.0f, 1.0f, fAlpha);
            for(unsigned int i = 0; i < vecDatas.size(); i++)
            {
                /*Space Between Objects*/
                if(i > unSpaceBetweenObjects)
                {
                    MainApp::Get().ptrRenderer->DrawLine(vecDatas.at(i-unSpaceBetweenObjects).z,
                                       vecDatas.at(i-unSpaceBetweenObjects).w,
                                       vecDatas.at(i).z, vecDatas.at(i).w, fThickness, shaderTexture, textureStreak);
                }

            }
            /*Count Of Objects*/
            if(vecDatas.size() > unCountOfObjects)
            {
                vecDatas.erase(vecDatas.begin() + 1);
            }
        }

        if(ImGui::IsMouseReleased(0))
        {
            if(!m_timerShowInfo.IsStarted())
            {
                m_timerShowInfo.Start(TIMER_ID_SHOW_INFO,unSpeedOfFade * 15);
            }

        }

        std::function<void()>callbackAlpha = [&]
        {
            fAlpha -= 0.1;
            if(fAlpha <= 0.0f)
            {
                m_timerShowInfo.Stop();
                vecDatas.clear();
            }
        };

        m_timerShowInfo.Tick(callbackAlpha);
    }
        break;

    default:
        break;
    }
}

void Debug::CurrentShader(int nShaderToApply)
{
    switch (nShaderToApply)
    {
    case eShaderWave:
    {
        m_vecShaders.at(nShaderToApply).setFloat("fTime", (float)glfwGetTime());
        m_vecShaders.at(nShaderToApply).setVec4("vecDatas", m_vec4ClipCrop);
    }
        break;

    case eShaderClip:
    {
        m_vecShaders.at(nShaderToApply).setVec4("vecClip", m_vec4ClipCrop);
    }
        break;

    case eShaderCrop:
    {
        m_vecShaders.at(nShaderToApply).setVec4("vecCrop", m_vec4ClipCrop);
    }
        break;

    case eShaderLens:
    {
        m_vecShaders.at(nShaderToApply).setVec4("vecDatas", m_vec4ClipCrop);
    }
        break;

    default:
        break;
    }
}

void Debug::EnableDisableDrag()
{
    ImGui::Checkbox("Enable Dragging", &m_bEnableDrag);

    if(!m_bEnableDrag)
    {
        return;
    }

    /* 0 - Mouse Left, 1 - Mouse Right */
    if(ImGui::IsMouseDown(0))
    {
        g_fXCoord = ImGui::GetMouseDragDelta(0).x + g_fXCoordMemory;
        g_fYCoord = ImGui::GetMouseDragDelta(0).y + g_fYCoordMemory;
    }
    else if(ImGui::IsMouseReleased(0))
    {
        g_fXCoordMemory = g_fXCoord;
        g_fYCoordMemory = g_fYCoord;
    }
}

void Debug::EnableDisableImageCut()
{
    ImGui::SameLine();
    ImGui::Checkbox("Enable Image Cut", &m_bEnableCut);
}

void Debug::SimulateRotation()
{
    if(!m_bSimulateRotation)
    {
        if(m_timerRotation.IsStarted())
        {
            m_timerRotation.Stop();
        }

        return;
    }

    if(!m_timerRotation.IsStarted())
    {
        m_timerRotation.Start(TIMER_ID_SIMULATE_ROTATION, 16);
    }
}

void Debug::TimerProcess()
{

    std::function<void()>callbackRotation = [&]
    {
        g_fAngle += m_fSpeedRotation;
        if(g_fAngle > 360.0f)
        {
            g_fAngle = 0.0f;
        }
        else if(g_fAngle < 0.0f)
        {
            g_fAngle = 360.0f;
        }
    };

    for(auto& objects : m_vecAnimData)
    {
        std::function<void()>callbackFrameObjects = [&]
        {
            if(objects.vecAnimation.empty())
            {
                return;
            }

            objects.nCurrentFrame++;
            if(objects.nCurrentFrame > objects.vecAnimation.size() - 1)
            {
                objects.nCurrentFrame = 0;
            }
        };

        objects.mTimerAnim.Tick(callbackFrameObjects);
    }

    std::function<void()>callbackShowPivot = [&]
    {
        if(m_bShowPivot)
        {
            m_fAlphaPivot += 0.1f;
            if(m_fAlphaPivot >= 1.0f)
            {
                m_fAlphaPivot = 1.0f;
                m_timerShowPivot.Stop();
            }
        }
        else
        {
            m_fAlphaPivot -= 0.03f;
            if(m_fAlphaPivot <= 0.0f)
            {
                m_fAlphaPivot = 0.0f;
                m_timerShowPivot.Stop();
            }
        }

    };

    m_timerRotation.Tick(callbackRotation);
    m_timerShowPivot.Tick(callbackShowPivot);

}

void Debug::CalculateMemoryOfPNGDDS()
{

    float fCurrCalcPNG = 0.0f;
    float fCurrCalcDDS = 0.0f;
    for(auto& objects : m_vecTextureData)
    {
        fCurrCalcPNG += (objects.ptrTexture->GetWidth() * objects.ptrTexture->GetHeight()) * 4;

        fCurrCalcDDS += ((objects.ptrTexture->GetWidth() + 3 - 1) / 3) *
                        ((objects.ptrTexture->GetHeight() + 3 - 1) / 3);

    }

    for(unsigned int i = 0; i < m_vecAnimData.size(); i++)
    {
        for(unsigned int j = 0; j < m_vecAnimData.at(i).vecAnimation.size(); j++)
        {
            fCurrCalcPNG += (m_vecAnimData.at(i).vecAnimation.at(j)->GetWidth() *
                          m_vecAnimData.at(i).vecAnimation.at(j)->GetHeight()) * 4;

            fCurrCalcDDS += ((m_vecAnimData.at(i).vecAnimation.at(j)->GetWidth() + 3 - 1) / 3) *
                            ((m_vecAnimData.at(i).vecAnimation.at(j)->GetHeight() + 3 - 1) / 3);
        }
    }

    m_fMemoryUsagePNG = fCurrCalcPNG / 1000000;
    m_fMemoryUsageDDS = fCurrCalcDDS / 100000;
}

void Debug::MainWindow()
{
    ImGui::Text("Texture Debug");
    ImGui::SameLine();
    ImGui::Text("RENDERING %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Text(", Resolution: %d x %d", MainApp::Get().ptrRenderer->SCREEN_WIDTH, MainApp::Get().ptrRenderer->SCREEN_HEIGHT);
    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text(", MEMORY USAGE (Hover Here...)");
    ImGui::EndGroup();
    if(ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("If textures are PNG's approximately: %f MB \nIf textures are DDS approximately: %f MB", m_fMemoryUsagePNG, m_fMemoryUsageDDS);
    }

    ImGui::BeginGroup();
    ImGui::SliderFloat("X Coords", &g_fXCoord, 0.0f, (float)MainApp::Get().ptrRenderer->SCREEN_WIDTH);
    ImGui::SliderFloat("Y Coords", &g_fYCoord, 0.0f, (float)MainApp::Get().ptrRenderer->SCREEN_HEIGHT);
    ImGui::SliderFloat("Angle Degrees", &g_fAngle, 0.0f, 360.0f);
    ImGui::SliderFloat("Scale Factor", &g_fScale, 0.0f, 1.0f);
    ImGui::EndGroup();
    if(m_bEnableDrag && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Drag Enabled !");
    }

    ImGui::NewLine();
    ImGui::InputFloat("X Precision", &g_fXCoord, 1.0f);
    ImGui::InputFloat("Y Precision", &g_fYCoord, 1.0f);
    ImGui::InputFloat("Angle Precision", &g_fAngle, 0.1f);
    ImGui::InputFloat("Scale Precision", &g_fScale, 0.01f);
    ImGui::ColorEdit4("Texture color", (float*)&m_vec4Color);
    ImGui::NewLine();
    ImGui::Checkbox("Texture Options", &m_bCreateTexture);
    ImGui::SameLine();
    ImGui::Checkbox("Animation Options", &m_bCreateAnimation);
    ImGui::SameLine();
    ImGui::Checkbox("Scene Options", &m_bSceneOptions);
    ImGui::SameLine();
    ImGui::Checkbox("Imgui HELP", &m_bShowImguiHelp);
}

void Debug::TextureAndRectModify()
{
    if(m_bEnable3D || !m_bEnableCut)
    {
        return;
    }
    const float fRectToFit = 350.0f;
    const float fWidthVertSliders = 20.0f;
    const float fOffsetHorizontalSliders = 5.0f;
    const float fPosClipX = 53.0f + ImGui::GetWindowPos().x;
    const float fPosClipY = 474.0f + ImGui::GetWindowPos().y;
    float fResizeValue = 1.0f;
    for(auto& currData: m_vecTextureData)
    {
        if(currData.bSelected)
        {
            if(currData.ptrTexture->GetOriginalWidth() > fRectToFit ||
               currData.ptrTexture->GetOriginalHeight() > fRectToFit)
            {
                fResizeValue = 3.0f;
            }

            g_fAngle -= 5.0f;
            if(g_fAngle <= 0.0f)
            {
                g_fAngle = 0.0f;
            }

            ImGui::BeginGroup();
            ImGui::NewLine();
            ImGui::Text("Selected: ");
            const float fWidth = (currData.ptrTexture->GetOriginalWidth() / fResizeValue);
            const float fHeight = (currData.ptrTexture->GetOriginalHeight() / fResizeValue);
            /*Fix of problem with STBI_LOAD flipped*/
            glm::vec2 vec2FlippedMinUV = glm::vec2(0.0f, 0.0f);
            glm::vec2 vec2FlippedMaxUV = glm::vec2(1.0f, 1.0f);

            if(currData.ptrTexture->GetFlipFormat() == Texture::EFlipFormat::eFlipVertical)
            {
                vec2FlippedMinUV = glm::vec2(0.0f, 1.0f);
                vec2FlippedMaxUV = glm::vec2(1.0f, 0.0f);
            }

            std::swap(vec2FlippedMinUV.y, vec2FlippedMaxUV.y);

            ImGui::PushItemWidth(fWidth);
            ImGui::NewLine();
            ImGui::SameLine();
            ImGui::Dummy({(fWidthVertSliders/2) + fOffsetHorizontalSliders, 0.0f});
            ImGui::SameLine();
            ImGui::Text(" ");
            ImGui::SameLine();
            ImGui::SliderInt("X", &currData.nDynamicSourceX, 0.0f, currData.ptrTexture->GetOriginalWidth());
            ImGui::NewLine();
            ImGui::VSliderInt(" Y", {fWidthVertSliders, fHeight}, &currData.nDynamicSourceY, currData.ptrTexture->GetOriginalHeight(), 0.0f);
            ImGui::SameLine();

            const float fCurrentPosClipX = fPosClipX + (currData.nDynamicSourceX / fResizeValue);

            const float fCurrentPosClipY = fPosClipY + (currData.nDynamicSourceY / fResizeValue);

            const float fCurrentPosClipW = (fPosClipX + fWidth + (currData.nDynamicSourceX / fResizeValue)) -
                    ((currData.ptrTexture->GetOriginalWidth() - currData.nDynamicSourceW) / fResizeValue);

            const float fCurrentPosClipH = (fPosClipY + fHeight + currData.nDynamicSourceY / fResizeValue) -
                    ((currData.ptrTexture->GetOriginalHeight() - currData.nDynamicSourceH) / fResizeValue);

            ImGui::PushClipRect({fCurrentPosClipX, fCurrentPosClipY}, {fCurrentPosClipW, fCurrentPosClipH}, true);
            ImGui::Image(ImTextureID((intptr_t)currData.ptrTexture->GetTextureID()),
                                      {fWidth, fHeight},
                                      {vec2FlippedMinUV.x, vec2FlippedMinUV.y},
                                      {vec2FlippedMaxUV.x, vec2FlippedMaxUV.y});
            ImGui::PopClipRect();

            ImGui::SameLine();
            ImGui::Text("H");
            ImGui::SameLine();
            ImGui::VSliderInt("", {fWidthVertSliders, fHeight}, &currData.nDynamicSourceH, currData.ptrTexture->GetOriginalHeight(), 0.0f);
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::SameLine();
            ImGui::Dummy({(fWidthVertSliders/2) + fOffsetHorizontalSliders, 0.0f});
            ImGui::SameLine();
            ImGui::Text(" ");
            ImGui::SameLine();
            ImGui::SliderInt("W", &currData.nDynamicSourceW, 0.0f, currData.ptrTexture->GetOriginalWidth());

            ImGui::Text("Previous values: X: %d Y: %d W: %d H: %d",
                        currData.ptrTexture->GetSourceX(),
                        currData.ptrTexture->GetSourceY(),
                        currData.ptrTexture->GetWidth(),
                        currData.ptrTexture->GetHeight());
            ImGui::EndGroup();
            if(m_bEnableDrag && ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Drag Enabled !");
            }

            break;
        }

    }

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.7f, 0.0f, 0.0f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1.0f, 0.0f, 0.0f, 1.0f});
    ImGui::BeginGroup();
    if(ImGui::Button("Apply Changes..."))
    {
        for(auto& currData : m_vecTextureData)
        {
            if(currData.bSelected)
            {
                glm::vec2 vecTopLeft;
                glm::vec2 vecTopRight;
                glm::vec2 vecBottomRight;
                glm::vec2 vecBottomLeft;

                /*DDS*/
                if(currData.ptrTexture->GetFlipFormat() == Texture::EFlipFormat::eFlipVertical)
                {

                    vecTopLeft = glm::vec2((float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                     ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight())));

                    vecTopRight = glm::vec2((float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth())+
                                                      (float)currData.nDynamicSourceW * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                      ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight())));

                    vecBottomRight = glm::vec2((float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth())+
                                                         (float)currData.nDynamicSourceW * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                         ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight()))+
                                                         (float)currData.nDynamicSourceH * (1/(float)currData.ptrTexture->GetOriginalHeight()));

                    vecBottomLeft = glm::vec2((float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                        ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight()))+
                                                        (float)currData.nDynamicSourceH * (1/(float)currData.ptrTexture->GetOriginalHeight()));

                }
                /*Others*/
                else
                {
                    vecTopLeft = glm::vec2({(float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                      1.0f - ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight()))});

                    vecTopRight = glm::vec2({(float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth())+
                                                       (float)currData.nDynamicSourceW * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                       1.0f - ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight()))});

                    vecBottomRight = glm::vec2({(float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth())+
                                                          (float)currData.nDynamicSourceW * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                          1.0f - ((float)currData.nDynamicSourceH * (1/(float)currData.ptrTexture->GetOriginalHeight()) +
                                                                 ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight())))});
                    vecBottomLeft = glm::vec2({(float)currData.nDynamicSourceX * (1/(float)currData.ptrTexture->GetOriginalWidth()),
                                                         1.0f - ((float)currData.nDynamicSourceH * (1/(float)currData.ptrTexture->GetOriginalHeight()) +
                                                                 ((float)currData.nDynamicSourceY * (1/(float)currData.ptrTexture->GetOriginalHeight())))});

                }

                currData.ptrTexture->SetTexCoord(Texture::ETexCoords::eTopLeft, vecTopLeft);
                currData.ptrTexture->SetTexCoord(Texture::ETexCoords::eTopRight, vecTopRight);
                currData.ptrTexture->SetTexCoord(Texture::ETexCoords::eBottomRight, vecBottomRight);
                currData.ptrTexture->SetTexCoord(Texture::ETexCoords::eBottomLeft, vecBottomLeft);

                currData.ptrTexture->SetSourceX(currData.nDynamicSourceX);
                currData.ptrTexture->SetSourceY(currData.nDynamicSourceY);
                currData.ptrTexture->SetSourceW(currData.nDynamicSourceW);
                currData.ptrTexture->SetSourceH(currData.nDynamicSourceH);

                m_bEnableCut = false;
                break;
            }
        }
    }
    ImGui::EndGroup();
    if(ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("The picture in preview will NOT change the size and UV");
    }
    ImGui::PopStyleColor(2);
}

void Debug::SimulateRotationLabel()
{
    ImGui::BeginGroup();
    ImGui::Checkbox("Simulate Rotation", &m_bSimulateRotation);
    ImGui::SameLine();
    ImGui::Checkbox("Enable 3D", &m_bEnable3D);
    if(m_bEnable3D)
    {
        ImGui::SameLine();
        ImGui::Checkbox("Rotate arond X Axis", &m_bRotateAroundAxisX);
        ImGui::SameLine();
        ImGui::Checkbox("Rotate arond Y Axis", &m_bRotateAroundAxisY);
        ImGui::SameLine();
        ImGui::Checkbox("Rotate around Z Axis", &m_bRotateAroundAxisZ);
        ImGui::BeginGroup();
        ImGui::SliderFloat("Z Coords", &g_fZCoord, -(float)MainApp::Get().ptrRenderer->SCREEN_DEPTH * 5, (float)MainApp::Get().ptrRenderer->SCREEN_DEPTH * 2);
        ImGui::EndGroup();
        if(m_bEnableDrag && ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Drag Enabled !");
        }

    }
    else
    {
        m_bRotateAroundAxisX = false;
        m_bRotateAroundAxisY = false;
        ImGui::SameLine();
        ImGui::Checkbox("Rotate around Z Axis", &m_bRotateAroundAxisZ);
    }
    ImGui::EndGroup();
    if(m_bEnableCut)
    {
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("When cut enabled, rotate simulation disabled");
        }

        m_bSimulateRotation = false;
    }

    if(!m_bRotateAroundAxisX && !m_bRotateAroundAxisY)
    {
        m_bRotateAroundAxisZ = true;
    }

    if(m_bSimulateRotation && !m_bEnableCut)
    {
        ImGui::SameLine();
        ImGui::Text(" Speed");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::BeginGroup();
        ImGui::DragFloat("-:+", &m_fSpeedRotation, 0.01f, -15.0f, 15.0f);
        if(!m_bEnable3D)
        {
            ImGui::SameLine();
            ImGui::Text(" Pivot");
            ImGui::SameLine();
            ImGui::DragFloat("Axis X", &g_fHorizontalAxisPivot, 0.1f, -1000.0f, 1000.0f);
            ImGui::SameLine();
            ImGui::DragFloat("Axis Y", &g_fVerticalAxisPivot, 0.1f, -1000.0f, 1000.0f);
        }
        ImGui::EndGroup();
        if(ImGui::IsItemHovered())
        {
            if(m_bEnableDrag)
            {
                ImGui::SetTooltip("Drag Enabled !");
            }
            else
            {
                if(!m_timerShowPivot.IsStarted())
                {
                    m_timerShowPivot.Start(TIMER_ID_PIVOT, 16);
                    m_bShowPivot = true;
                }
            }

        }
        else
        {
            if(!m_timerShowPivot.IsStarted())
            {
                m_timerShowPivot.Start(TIMER_ID_PIVOT, 16);
                m_bShowPivot = false;
            }
        }
    }
    else
    {
        g_fHorizontalAxisPivot = 0.0f;
        g_fVerticalAxisPivot = 0.0f;
    }

    /*Simulate Rotation*/
    SimulateRotation();
}

void Debug::Process()
{
    /*Draw Textures and Animations*/
    Draw();

    /*Timer Process call function*/
    TimerProcess();

    /*Write into txt file all the data*/
    WriteIntoFile();

    /*Main Window*/
    MainWindow();

    /*Enable Disable Drag function*/
    EnableDisableDrag();

    /*Enable Disable Image Cut function*/
    EnableDisableImageCut();

    /*Simulate Rotation function*/
    SimulateRotationLabel();

    /*Shaders Menu*/
    EnableDisableShader();

    /*Small Window texture preview*/
    TextureAndRectModify();

    /*Selectable Radio Button Textures*/
    RadioButtonTextures();

    /*Right Click select/deselect texture*/
    RightClickAndSelect();

    //Help IMGUI
    if (m_bShowImguiHelp)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&m_bShowImguiHelp);
    }

    //Create Texture
    CreateTextureAndTextureOptions();

    //Create Animations
    CreateAnimAndAnimOpitions();

    /*Scene Menu*/
    SceneMenu();

    /*PNG DDS Memory usage*/
    CalculateMemoryOfPNGDDS();

    ImGui::Render();

    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Debug::RightClickAndSelect()
{
    int xMouse = ImGui::GetMousePos().x;
    int yMouse = ImGui::GetMousePos().y;

    /*Right Mouse Button*/
    for (unsigned int i = m_vecTextureData.size(); i-- > 0; )
    {
        bool bInCoords = (xMouse >= m_vecTextureData.at(i).fX) &&
                         (xMouse <= m_vecTextureData.at(i).fX + m_vecTextureData.at(i).ptrTexture->GetWidth()) &&
                         (yMouse >= m_vecTextureData.at(i).fY) &&
                         (yMouse <= m_vecTextureData.at(i).fY + m_vecTextureData.at(i).ptrTexture->GetHeight());

        if(ImGui::IsMouseClicked(1) && bInCoords)
        {
            g_fXCoord = m_vecTextureData.at(i).fX;
            g_fYCoord = m_vecTextureData.at(i).fY;
            g_fXCoordMemory = m_vecTextureData.at(i).fX;
            g_fYCoordMemory = m_vecTextureData.at(i).fY;
            g_fAngle = m_vecTextureData.at(i).fAngle;
            g_fScale = m_vecTextureData.at(i).fScaleFactor;
            m_vecTextureData.at(i).bSelected = !m_vecTextureData.at(i).bSelected;
            m_vecTextureData.at(i).nDynamicSourceX = m_vecTextureData.at(i).ptrTexture->GetSourceX();
            m_vecTextureData.at(i).nDynamicSourceY = m_vecTextureData.at(i).ptrTexture->GetSourceY();
            m_vecTextureData.at(i).nDynamicSourceW = m_vecTextureData.at(i).ptrTexture->GetWidth();
            m_vecTextureData.at(i).nDynamicSourceH = m_vecTextureData.at(i).ptrTexture->GetHeight();
            break;
        }

    }
}

void Debug::Draw()
{
    MainApp::Get().ptrRenderer->SendVertsToVBODebug();
    MainApp::Get().ptrRenderer->SendTextureCoordsToVBODebug();

    /*Textures from Debug*/
    for(auto& objects : m_vecTextureData)
    {
        /*Rotation Pivots*/
        float fHorizontalAxis = 0.0f;
        float fVerticalAxis = 0.0f;

        if(objects.bSelected)
        {
            MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,0.7f);
            fHorizontalAxis = g_fHorizontalAxisPivot;
            fVerticalAxis = g_fVerticalAxisPivot;
        }
        else
        {
            MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,1.0f);
        }


        /* Check Debug.cpp:EnableDisableShader() to add more Shaders
           Currently - "NONE", "Wave"*/
        if(m_bEnable3D)
        {
            float fXNormalized = ((2 * objects.fX) / MainApp::Get().ptrRenderer->SCREEN_WIDTH) - 1;
            float fYNormalized = ((2 * objects.fY) / MainApp::Get().ptrRenderer->SCREEN_WIDTH) - 1;
            float fZNormalized = ((2 * g_fZCoord) / (MainApp::Get().ptrRenderer->SCREEN_DEPTH)) - 1;
            fYNormalized = -1 * fYNormalized;

            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(fXNormalized, fYNormalized, fZNormalized));
            model = glm::rotate(model, glm::radians(objects.fAngle * -1), glm::vec3((float)m_bRotateAroundAxisX,
                                                                                    (float)m_bRotateAroundAxisY,
                                                                                    (float)m_bRotateAroundAxisZ));
            view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            MainApp::Get().ptrRenderer->DrawPicture3D(objects.ptrTexture, shaderTexture3D, model, view);

            continue;
        }

        MainApp::Get().ptrRenderer->DrawPictureDebug(objects.ptrTexture,
                                   objects.fX,
                                   objects.fY,
                                   objects.fScaleFactor,
                                   objects.fAngle,
                                   fHorizontalAxis,
                                   fVerticalAxis,
                                   m_bRotateAroundAxisX,
                                   m_bRotateAroundAxisY,
                                   m_bRotateAroundAxisZ,
                                   m_vecShaders.at(objects.unShaderID));

        if(objects.bSelected && m_bEnableCut)
        {
            const float fXRect = objects.fX + (float)objects.nDynamicSourceX;
            const float fYRect = objects.fY + (float)objects.nDynamicSourceY;
            const float fWRect = objects.ptrTexture->GetWidth() - (objects.ptrTexture->GetWidth() - (float)objects.nDynamicSourceW);
            const float fHRect = objects.ptrTexture->GetHeight() - (objects.ptrTexture->GetHeight() - (float)objects.nDynamicSourceH);

            if(fXRect + fWRect > objects.fX + objects.ptrTexture->GetOriginalWidth() ||
               fYRect + fHRect > objects.fY + objects.ptrTexture->GetOriginalHeight())
            {
                MainApp::Get().ptrRenderer->SetColor(1.0f,0.0f,0.0f,0.7f);
            }
            else
            {
                MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,0.7f);
            }

            MainApp::Get().ptrRenderer->DrawRect(fXRect, fYRect, fWRect, fHRect, shaderRect);
        }

        if(objects.bSelected && m_bSimulateRotation && m_fAlphaPivot > 0.0f)
        {
            float fXPivot = (objects.fX + (objects.ptrTexture->GetWidth() / 2) - (textureX->GetWidth() / 2));
            float fYPivot = (objects.fY + (objects.ptrTexture->GetHeight() / 2) - (textureX->GetHeight() / 2));
            MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,m_fAlphaPivot);
            MainApp::Get().ptrRenderer->DrawPicture(textureX, fXPivot, fYPivot, shaderTexture);
        }

        CurrentShader(objects.unShaderID);
    }

    /*Animations from Debug*/
    if(!m_vecAnimData.empty())
    {
        for(auto& objects : m_vecAnimData)
        {
            if(objects.bSelected)
            {
                MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,0.7f);
            }
            else
            {
                MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,1.0f);
            }

            unsigned int nFrame = objects.nCurrentFrame;
            MainApp::Get().ptrRenderer->DrawPictureDebug(objects.vecAnimation.at(nFrame),
                                       objects.fX,
                                       objects.fY,
                                       objects.fScaleFactor,
                                       objects.fAngle,
                                       0.0f,
                                       0.0f,
                                       m_bRotateAroundAxisX,
                                       m_bRotateAroundAxisY,
                                       m_bRotateAroundAxisZ,
                                       shaderTexture);
        }

    }

    MainApp::Get().ptrRenderer->SetColor(1.0f,1.0f,1.0f,1.0f);

    CurrentEffect(m_nCurrentEffect);

}

void Debug::NewFrame()
{
    ImGui_ImplGlfwGL3_NewFrame();
}

void Debug::Deinit()
{
    ImGui::DestroyContext();
    m_vecTextureData.clear();
    m_vecAnimData.clear();
    m_vecStrFiles.clear();
}
