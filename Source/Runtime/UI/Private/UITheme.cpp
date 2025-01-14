#include "U

#include "Runtime/Core/Public/Core/UICore.h"ITheme.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace VGE {
namespace Editor {

UITheme UITheme::s_CurrentTheme;

void UITheme::LoadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open theme file: " + filepath);
        }

        nlohmann::json j;
        file >> j;

        // Load colors
        auto& colors = j["colors"];
        m_Colors.Background = LoadColor(colors, "background");
        m_Colors.BackgroundDark = LoadColor(colors, "backgroundDark");
        m_Colors.BackgroundLight = LoadColor(colors, "backgroundLight");
        m_Colors.Text = LoadColor(colors, "text");
        m_Colors.TextDisabled = LoadColor(colors, "textDisabled");
        m_Colors.Border = LoadColor(colors, "border");
        m_Colors.BorderSelected = LoadColor(colors, "borderSelected");
        m_Colors.Button = LoadColor(colors, "button");
        m_Colors.ButtonHovered = LoadColor(colors, "buttonHovered");
        m_Colors.ButtonActive = LoadColor(colors, "buttonActive");
        m_Colors.ButtonDisabled = LoadColor(colors, "buttonDisabled");
        m_Colors.Header = LoadColor(colors, "header");
        m_Colors.HeaderHovered = LoadColor(colors, "headerHovered");
        m_Colors.HeaderActive = LoadColor(colors, "headerActive");
        m_Colors.Accent = LoadColor(colors, "accent");
        m_Colors.AccentHovered = LoadColor(colors, "accentHovered");
        m_Colors.AccentActive = LoadColor(colors, "accentActive");
        m_Colors.Error = LoadColor(colors, "error");
        m_Colors.Warning = LoadColor(colors, "warning");
        m_Colors.Success = LoadColor(colors, "success");

        // Load style
        auto& style = j["style"];
        m_Style.WindowPadding = style["windowPadding"].get<float>();
        m_Style.WindowRounding = style["windowRounding"].get<float>();
        m_Style.WindowBorderSize = style["windowBorderSize"].get<float>();
        m_Style.WindowTitleAlign = style["windowTitleAlign"].get<float>();
        m_Style.ChildRounding = style["childRounding"].get<float>();
        m_Style.ChildBorderSize = style["childBorderSize"].get<float>();
        m_Style.PopupRounding = style["popupRounding"].get<float>();
        m_Style.PopupBorderSize = style["popupBorderSize"].get<float>();
        m_Style.FramePadding = style["framePadding"].get<float>();
        m_Style.FrameRounding = style["frameRounding"].get<float>();
        m_Style.FrameBorderSize = style["frameBorderSize"].get<float>();
        m_Style.ItemSpacing = style["itemSpacing"].get<float>();
        m_Style.ItemInnerSpacing = style["itemInnerSpacing"].get<float>();
        m_Style.TouchExtraPadding = style["touchExtraPadding"].get<float>();
        m_Style.IndentSpacing = style["indentSpacing"].get<float>();
        m_Style.ScrollbarSize = style["scrollbarSize"].get<float>();
        m_Style.ScrollbarRounding = style["scrollbarRounding"].get<float>();
        m_Style.GrabMinSize = style["grabMinSize"].get<float>();
    }
    catch (const std::exception& e) {
        // TODO: Add proper logging
        throw std::runtime_error("Failed to load theme: " + std::string(e.what()));
    }
}

void UITheme::SaveToFile(const std::string& filepath) const {
    try {
        nlohmann::json j;

        // Save colors
        j["colors"]["background"] = SaveColor(m_Colors.Background);
        j["colors"]["backgroundDark"] = SaveColor(m_Colors.BackgroundDark);
        j["colors"]["backgroundLight"] = SaveColor(m_Colors.BackgroundLight);
        j["colors"]["text"] = SaveColor(m_Colors.Text);
        j["colors"]["textDisabled"] = SaveColor(m_Colors.TextDisabled);
        j["colors"]["border"] = SaveColor(m_Colors.Border);
        j["colors"]["borderSelected"] = SaveColor(m_Colors.BorderSelected);
        j["colors"]["button"] = SaveColor(m_Colors.Button);
        j["colors"]["buttonHovered"] = SaveColor(m_Colors.ButtonHovered);
        j["colors"]["buttonActive"] = SaveColor(m_Colors.Button��MHW�AS�W��4r]\Y.h���G�m��������S����)����%FY���E5��e�׋�N(�8��_���t)��F��#��������������p\5�S������9F�����啓�&�Eu������3i���1��,C��F�N?�-��D�L=S�zL���M�59�o�)�Va�,_ʭM�w����<�-f��]ٚ���쪼�`u1�z��YM'�X��H���s�b������?N��3`e|��l+�t��]o�H��=2�G!�G�tn��<\���I�X9��k�:~M���ꚨ�`���/6w�";N�>�̿��$���&�{�\c�Z�y�%j������p�"��K�g��*ӕ�3��� �`0�'%�@'��`���~��nX��=����S�S�f��\S�������?M��[`j|�1]2P�h�D��\
9,���{%0���u`MM4C���1�7؝K)��`>`(��(��m�ر\�JϦ
������Pc�|�(�%�%x�8�@��Y���]9c;�&��]=�(07�ɩUS?���h���b�rA+��G�m�رaWv�@�=���<A9v��e!��G��NO
fy��&����`2'i
~`
z'ѽF��)�����^���AS"���o�+���z�������q�+�,~	Ie*F��sN,Cm�Ŗ3.���u�ߴ��@����D�Sp\O8`/t��5��a#�Nj:	�T@W�3�\�Y.�#�o�0���)����
�E����huO_���ݩ��ja4�N�v_]�W�����f��8trmM����m2��p �-d�I
q��Z�I�Π/&���uqf������L������;��(K]g�5]z��թ,ǜD����O�n}r-�\�7�7jM Zh��yţ��E����(��2�9��&ڟ⯿3��#̌��>NK�ۧh��}7љ��E��Y.h��f>dJ���?M\���&��໹�%�W�~S���6&�}�E��l�c�l�����U��:��sK�S��������C����4�~�N���?�Z鞦o�q����u��K�wӣ�~;q?l�>��灻��7���J��4%��%�O4�P�k��4Czs��� �����LA1���.�,��g:%�o���m�)�T�*��e)Q,q�����Ӱ��WE+R)�^m��#}9Sb	;�_���$�KP`���u�d����Í5S"�X��Xb8��<��bS�P����v��x����Vt�NѪ�i�Pz�C�����d�N<��hg?}���Vk����`Vc�2%���?�;@�SM={�Z)�j�D�`ob���H�t�x#kL�����]�4c���`oEs���g�5Q��Oki�����C6r���#�_h
~��r��.�,�ҽ���M����@���f#��s��yd��]��L�fr�`�s��~���	�ɜ��n�p����`����GS����(0�\?�\�J�ONRۈ������'T¥ʱ&E�,.+�4�`gTK���_�M��Ҿ9�Қ��`����h|��`�"����M�5?|���F�������]�?|J��o8F����V��d���w�����yN��"��ߝ�4�.��:L�g	M�"o2�7�2�ah�#4?0�%:��s1f���>�:Fm��*�M{I�y�E�k��ХĻf����s�Zo�{z�*P�\�6q���c���(�`������ Ś=2r]�\�Jǣc�6���p�1��P�+՗e�g`�z�Z(�hdȼ��+3q��D�+�o�?���#�F=%��o$e�
�kE��/l�2�Z��6V�g>#Q���i��c�h�י-�]_�I�����/@@��=ҧ�RS����_!:��:��?���\uף�D�C����s<�=�TU��s1O9~���/�3�w vo 
)�Ϳ��f�/�	,���њ���&
��]�\�J?�9o&���?9���,䳃�ʟt�t�ŜCݽk׎��6��B���0���f�c�	# }y�tk��ޙ�+�Ϳ�ߜ3��2��h�c4?|��D/��oX.h�����m�|�1��M�ۄO������G9��0&�S�\y>:�2z�d�V
��`|���@�$�A"�JS��G�[�n7r]�\�J�[j���3Q;�Ǌ�G%�}����jj3}#v�]+��>�C��M�q�������lD��:M��A_Jt�����rA+�NR��7pf{qF ����+o�:�x�,����rz_�`Kf%�w�l3�'
�x �����>}�MF����8���_3%Bl�'�'x�l�������k�ǎ�#�1�#���]��}�iRp�kp;bp���tFz�����d@ۉ�0�\�X.(}��t�x�iH���C�2�f�'�J�:>�/pĭ���њ��`b���?,����)��ȯ�(:	����h���b�rA+}?�,��K�S�.A��5D��Quu�O��|������[qj����A�����Q���x������&*��_�5�إ�V������)���W=d亾`��������!&�E��� 2��p�ܯ������n)