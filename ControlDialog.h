#pragma once
#ifndef _CONTROLDIALOG_H_
#define _CONTROLDIALOG_H_

#include "C:\\MyDirC\\Code\\cpp\\usetool\\usetool7.1\\usetool.h"
#include "resource.h"
#include "Worker.h"

class ControlDialog : protected IDialog
{
protected:
	struct EditStruct
	{
	public:
		//body
		float time_rate;
		//particle
		int particle_count;
		float particle_distance;
		int particle_offset_x;
		int particle_offset_y;
		int particle_offset_z;
		//kerr
		int kerr_mass;
		float kerr_a_rate;
		float kerr_position[3];
		float kerr_speed;
		//light
		int light_count;
		int light_min;
		int light_max;
		//sky
		int sky_density;
		//scale
		float scale_time;
		int scale_space;
		int fov;
		//move
		float speed_move;
		float speed_spin;
		float position[3];
	public:
		bool operator==(const ControlDialog::EditStruct& edit) const
		{
			return !std::memcmp(this, &edit, sizeof(ControlDialog::EditStruct));
		}
	};

protected:
	IWinUI* pWindow;

	Worker& worker;

	ControlDialog::EditStruct editInit;
	ControlDialog::EditStruct editChange;

	int chooseId;

public:
	bool Create(HWND hWndParent)
	{
		return IDialog::CreateNoMode(hWndParent, LPARAM{});
	}

	const HWND& HDlg()
	{
		return IDialog::hDlg;
	}

protected:
	void UpdataBodyListBox()
	{
		static auto makeList{ [](_IN_ const BodyManager::BodyStruct& body) -> std::string
		{
			std::string text(500ULL, ' '); //must (), no {}. 468max
			//id
			std::string temp{ std::to_string(body.id) };
			text.replace(0ULL, (temp.size() * 2ULL), temp);
			//mass
			std::ostringstream oss{};
			oss << std::scientific << std::setprecision(2LL) << std::uppercase;
			oss << body.mass;
			temp = oss.str();
			text.replace(21ULL, (temp.size() * 2ULL), temp);
			//radius
			oss.str("");
			oss << (body.radius * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(55ULL, (temp.size() * 2ULL), temp);
			//position
			oss.str("");
			oss << (body.position[0][0] * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(96ULL, (temp.size() * 2ULL), temp);
			oss.str("");
			oss << (body.position[0][1] * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(138ULL, (temp.size() * 2ULL), temp);
			oss.str("");
			oss << (body.position[0][2] * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(180ULL, (temp.size() * 2ULL), temp);
			//velocity
			oss.str("");
			oss << (body.velocity[0][0] * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(222ULL, (temp.size() * 2ULL), temp);
			oss.str("");
			oss << (body.velocity[0][1] * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(263ULL, (temp.size() * 2ULL), temp);
			oss.str("");
			oss << (body.velocity[0][2] * NGlobal::scale_distance);
			temp = oss.str();
			text.replace(306ULL, (temp.size() * 2ULL), temp);
			//color
			temp = std::to_string(static_cast<int>(body.color[0] * 255.F));
			text.replace(340ULL, (temp.size() * 2ULL), temp);
			temp = std::to_string(static_cast<int>(body.color[1] * 255.F));
			text.replace(365ULL, (temp.size() * 2ULL), temp);
			temp = std::to_string(static_cast<int>(body.color[2] * 255.F));
			text.replace(385ULL, (temp.size() * 2ULL), temp);
			return text;
		} };
		std::vector<std::string> texts{};
		const std::vector<BodyManager::BodyStruct> bodies{ ControlDialog::worker.BodiesData() };
		for (const BodyManager::BodyStruct& body : bodies)
		{
			texts.emplace_back(makeList(body));
		}
		IDialog::ListBoxSetData(texts, IDC_BODY_LIST);
	}
	void Init()
	{
		static auto setInt{ [&](int idEdit, int num) -> void
			{
				IDialog::SetWindowTextA(IDialog::GetDlgItem(idEdit), std::to_string(num));
			} };
		static auto setFloat{ [&](int idEdit, float num) -> void
			{
				std::string str{ std::to_string(num) };
				str.erase(str.find_last_not_of('0') + 1ULL);
				IDialog::SetWindowTextA(IDialog::GetDlgItem(idEdit), str);
			} };
		static auto mag{ [](float num) -> int
			{
				return static_cast<int>(std::round(std::log10f(std::abs(num))));
			} };
		static auto setCheck{ [&](int idCheck, bool check) -> void
			{
				Button_SetCheck(IDialog::GetDlgItem(idCheck), (check ? BST_CHECKED : BST_UNCHECKED));
			} };
		//body
		ControlDialog::UpdataBodyListBox();
		setFloat(IDC_TIME_RATE, (ControlDialog::editInit.time_rate = NGlobal::time_calculate_rate));
		//particle
		const ParticleManager::ParticleData particle{ ControlDialog::worker.ParticleData() };
		setInt(IDC_PARTICLE_COUNT, (ControlDialog::editInit.particle_count = particle.particle_count));
		setFloat(IDC_PARTICLE_DISTANCE, (ControlDialog::editInit.particle_distance = particle.particle_distance));
		setInt(IDC_PARTICLE_OFFSETX, (ControlDialog::editInit.particle_offset_x = static_cast<int>(particle.particle_offset_x)));
		setInt(IDC_PARTICLE_OFFSETY, (ControlDialog::editInit.particle_offset_y = static_cast<int>(particle.particle_offset_y)));
		setInt(IDC_PARTICLE_OFFSETZ, (ControlDialog::editInit.particle_offset_z = static_cast<int>(particle.particle_offset_z)));
		//kerr
		const KerrManager::KerrStruct kerr{ ControlDialog::worker.KerrData() };
		setCheck(IDC_KERR_HAS, ControlDialog::worker.KerrHas());
		setCheck(IDC_DISK_HAS, ControlDialog::worker.KerrDisk());
		setInt(IDC_KERR_MASS, (ControlDialog::editInit.kerr_mass = static_cast<int>(std::round(kerr.kerr_mass / NConst::sun_mass))));
		setFloat(IDC_KERR_A_RATE, (ControlDialog::editInit.kerr_a_rate = kerr.kerr_a_rate));
		setFloat(IDC_KERR_X, (ControlDialog::editInit.kerr_position[0] = kerr.kerr_position[0]));
		setFloat(IDC_KERR_Y, (ControlDialog::editInit.kerr_position[1] = kerr.kerr_position[1]));
		setFloat(IDC_KERR_Z, (ControlDialog::editInit.kerr_position[2] = kerr.kerr_position[2]));
		setFloat(IDC_KERR_SPEED, (ControlDialog::editInit.kerr_speed = kerr.disk_speed));
		//light
		const LightTrace::LightStruct light{ ControlDialog::worker.LightData() };
		setCheck(IDC_LIGHT, ControlDialog::worker.LightGetIs());
		setInt(IDC_LIGHT_STEP_COUNT, (ControlDialog::editInit.light_count = static_cast<int>(light.light_step_count)));
		setInt(IDC_LIGHT_STEP_MIN, (ControlDialog::editInit.light_min = mag(light.light_step_min)));
		setInt(IDC_LIGHT_STEP_MAX, (ControlDialog::editInit.light_max = mag(light.light_step_max)));
		//sky
		setCheck(IDC_SKYBOX, ControlDialog::worker.SkyHas());
		setInt(IDC_SKYBOX_DENSITY, (ControlDialog::editInit.sky_density = ControlDialog::worker.SkyDensity()));
		//scale
		setFloat(IDC_SCALE_TIME, (ControlDialog::editInit.scale_time = NGlobal::scale_time));
		setInt(IDC_SCALE_SPACE, (ControlDialog::editInit.scale_space = mag(NGlobal::scale_distance)));
		setInt(IDC_FOV, (ControlDialog::editInit.fov = static_cast<int>(ControlDialog::pWindow->GetCamera().GetFovAngleY())));
		//move
		setFloat(IDC_SPEED_MOVE, (ControlDialog::editInit.speed_move = ControlDialog::pWindow->GetCamera().GetMoveSpeed()));
		setFloat(IDC_SPEED_SPIN, (ControlDialog::editInit.speed_spin = ControlDialog::pWindow->GetCamera().GetSpinSpeed()));
		IWinUI::Position position{ ControlDialog::pWindow->GetCamera().GetPosition() };
		setFloat(IDC_POSITION_X, (ControlDialog::editInit.position[0] = position.x));
		setFloat(IDC_POSITION_Y, (ControlDialog::editInit.position[1] = position.y));
		setFloat(IDC_POSITION_Z, (ControlDialog::editInit.position[2] = position.z));
		//finish
		ControlDialog::editChange = ControlDialog::editInit;
	}

	void EditChange(WORD idItem)
	{
		std::string text{};
		IDialog::GetDlgItemTextA(idItem, text, 100);
		switch (idItem)
		{
			//body
		case IDC_TIME_RATE: ControlDialog::editChange.time_rate = NString::ToFloat(text); break;
			//particle
		case IDC_PARTICLE_COUNT: ControlDialog::editChange.particle_count = NString::ToInt(text); break;
		case IDC_PARTICLE_DISTANCE: ControlDialog::editChange.particle_distance = NString::ToFloat(text); break;
		case IDC_PARTICLE_OFFSETX: ControlDialog::editChange.particle_offset_x = NString::ToInt(text); break;
		case IDC_PARTICLE_OFFSETY: ControlDialog::editChange.particle_offset_y = NString::ToInt(text); break;
		case IDC_PARTICLE_OFFSETZ: ControlDialog::editChange.particle_offset_z = NString::ToInt(text); break;
			//kerr
		case IDC_KERR_MASS: ControlDialog::editChange.kerr_mass = NString::ToInt(text); break;
		case IDC_KERR_A_RATE: ControlDialog::editChange.kerr_a_rate = NString::ToFloat(text); break;
		case IDC_KERR_X: ControlDialog::editChange.kerr_position[0] = NString::ToFloat(text); break;
		case IDC_KERR_Y: ControlDialog::editChange.kerr_position[1] = NString::ToFloat(text); break;
		case IDC_KERR_Z: ControlDialog::editChange.kerr_position[2] = NString::ToFloat(text); break;
		case IDC_KERR_SPEED: ControlDialog::editChange.kerr_speed = NString::ToFloat(text); break;
			//light
		case IDC_LIGHT_STEP_COUNT: ControlDialog::editChange.light_count = NString::ToInt(text); break;
		case IDC_LIGHT_STEP_MIN: ControlDialog::editChange.light_min = NString::ToInt(text); break;
		case IDC_LIGHT_STEP_MAX: ControlDialog::editChange.light_max = NString::ToInt(text); break;
			//sky
		case IDC_SKYBOX_DENSITY: ControlDialog::editChange.sky_density = NString::ToInt(text); break;
			//scale
		case IDC_SCALE_TIME: ControlDialog::editChange.scale_time = NString::ToFloat(text); break;
		case IDC_SCALE_SPACE: ControlDialog::editChange.scale_space = NString::ToInt(text); break;
		case IDC_FOV: ControlDialog::editChange.fov = NString::ToInt(text); break;
			//move
		case IDC_SPEED_MOVE: ControlDialog::editChange.speed_move = NString::ToFloat(text); break;
		case IDC_SPEED_SPIN: ControlDialog::editChange.speed_spin = NString::ToFloat(text); break;
		case IDC_POSITION_X: ControlDialog::editChange.position[0] = NString::ToFloat(text); break;
		case IDC_POSITION_Y: ControlDialog::editChange.position[1] = NString::ToFloat(text); break;
		case IDC_POSITION_Z: ControlDialog::editChange.position[2] = NString::ToFloat(text); break;
		}
	}
	void Apply()
	{
		if (ControlDialog::editChange == ControlDialog::editInit) return;
		//body
		if (ControlDialog::editChange.time_rate != ControlDialog::editInit.time_rate)
		{

		}
		//particle
		if ((ControlDialog::editChange.particle_count != ControlDialog::editInit.particle_count) ||
			(ControlDialog::editChange.particle_distance != ControlDialog::editInit.particle_distance) ||
			(ControlDialog::editChange.particle_offset_x != ControlDialog::editInit.particle_offset_x) ||
			(ControlDialog::editChange.particle_offset_y != ControlDialog::editInit.particle_offset_y) ||
			(ControlDialog::editChange.particle_offset_z != ControlDialog::editInit.particle_offset_z))
		{
			ControlDialog::worker.ParticleChange(ControlDialog::editChange.particle_count, ControlDialog::editChange.particle_distance,
				static_cast<float>(ControlDialog::editChange.particle_offset_x),
				static_cast<float>(ControlDialog::editChange.particle_offset_y),
				static_cast<float>(ControlDialog::editChange.particle_offset_z));
		}
		//kerr
		if ((ControlDialog::editChange.kerr_mass != ControlDialog::editInit.kerr_mass) ||
			(ControlDialog::editChange.kerr_a_rate != ControlDialog::editInit.kerr_a_rate) ||
			(ControlDialog::editChange.kerr_position[0] != ControlDialog::editInit.kerr_position[0]) ||
			(ControlDialog::editChange.kerr_position[1] != ControlDialog::editInit.kerr_position[1]) ||
			(ControlDialog::editChange.kerr_position[2] != ControlDialog::editInit.kerr_position[2]) ||
			(ControlDialog::editChange.kerr_speed != ControlDialog::editInit.kerr_speed))
		{
			ControlDialog::worker.KerrChange(ControlDialog::worker.KerrHas(),
				(ControlDialog::editChange.kerr_mass * NConst::sun_mass), ControlDialog::editChange.kerr_a_rate,
				ControlDialog::editChange.kerr_position[0], ControlDialog::editChange.kerr_position[1], ControlDialog::editChange.kerr_position[2],
				ControlDialog::worker.KerrDisk(), ControlDialog::editChange.kerr_speed);
		}
		//light
		if ((ControlDialog::editChange.light_count != ControlDialog::editInit.light_count) ||
			(ControlDialog::editChange.light_min != ControlDialog::editInit.light_min) ||
			(ControlDialog::editChange.light_max != ControlDialog::editInit.light_max))
		{
			ControlDialog::worker.LightChange(static_cast<unsigned int>(ControlDialog::editChange.light_count),
				::powf(10.F, static_cast<float>(ControlDialog::editChange.light_min)),
				::powf(10.F, static_cast<float>(ControlDialog::editChange.light_max)));
		}
		//sky
		if (ControlDialog::editChange.sky_density != ControlDialog::editInit.sky_density)
		{
			ControlDialog::worker.SkyChange(ControlDialog::worker.SkyHas(), ControlDialog::worker.SkyPath(), ControlDialog::editChange.sky_density);
		}
		//scale
		if (ControlDialog::editChange.scale_time != ControlDialog::editInit.scale_time)
		{
			ControlDialog::worker.ChangeScaleTime(ControlDialog::editChange.scale_time);
		}
		if (ControlDialog::editChange.scale_space != ControlDialog::editInit.scale_space)
		{
			ControlDialog::worker.ChangeScaleDistance(::powf(10.F, static_cast<float>(ControlDialog::editChange.scale_space)));
		}
		if (ControlDialog::editChange.fov != ControlDialog::editInit.fov)
		{
			ControlDialog::pWindow->GetCamera().SetFov(static_cast<float>(ControlDialog::editChange.fov));
		}
		//move
		if ((ControlDialog::editChange.speed_move != ControlDialog::editInit.speed_move) ||
			(ControlDialog::editChange.speed_spin != ControlDialog::editInit.speed_spin))
		{
			ControlDialog::pWindow->GetCamera().SetMoveSpeed(ControlDialog::editChange.speed_move);
			ControlDialog::pWindow->GetCamera().SetSpinSpeed(ControlDialog::editChange.speed_spin);
		}
		if ((ControlDialog::editChange.position[0] != ControlDialog::editInit.position[0]) ||
			(ControlDialog::editChange.position[1] != ControlDialog::editInit.position[1]) ||
			(ControlDialog::editChange.position[2] != ControlDialog::editInit.position[2]))
		{
			ControlDialog::pWindow->GetCamera().SetPosition(IWinUI::Position{
				ControlDialog::editChange.position[0], ControlDialog::editChange.position[1], ControlDialog::editChange.position[2] });
		}
		ControlDialog::editInit = ControlDialog::editChange;
	}

	void Command(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static auto getCheck{ [&](int idCheck) -> bool
			{
				return (Button_GetCheck(IDialog::GetDlgItem(idCheck)) == BST_CHECKED);
			} };
		if (IDialog::ListBoxIsChange(IDC_BODY_LIST, uMsg, wParam, lParam))
		{
			ControlDialog::chooseId = ControlDialog::worker.BodiesData().at(IDialog::ListBoxGetIndex(IDC_BODY_LIST)).id;
			return;
		}
		WORD idItem{ LOWORD(wParam) };
		WORD msgItem{ HIWORD(wParam) };
		if (msgItem == EN_CHANGE)
		{
			ControlDialog::EditChange(idItem);
			return;
		}
		if (msgItem == BN_CLICKED)
		{
			switch (idItem)
			{
			//body
			case IDC_BODY_ADD:
			{
				BodyManager::BodyStruct body8{ 8U,1E3,1E7,0,0,0,0,0,1E5,1,0,0 };
				if (ControlDialog::worker.BodyAdd(body8)) ControlDialog::UpdataBodyListBox();
			} break;
			case IDC_BODY_DELETE:
			{
				if (ControlDialog::chooseId < 0) break;
				if (ControlDialog::worker.BodyDelete(static_cast<unsigned int>(ControlDialog::chooseId))) ControlDialog::UpdataBodyListBox();
			} break;
			case IDC_BODY_CLEAR:
			{
				ControlDialog::worker.BodyClear();
				ControlDialog::UpdataBodyListBox();
			} break;
			case IDC_FOLLOW:; break;
			case IDC_FOCUS_BODY:; break;
			case IDC_FOCUS_KERR:
			{
				ControlDialog::worker.FocusKerr();
			} break;
			case IDC_UNFOLLOW:
			{
				ControlDialog::worker.Follow(false, ControlDialog::chooseId);
			} break;
			case IDC_UNFOCUS:
			{
				ControlDialog::worker.UnFocus();
			} break;
			//kerr
			case IDC_KERR_HAS:
			case IDC_DISK_HAS:
			{
				const KerrManager::KerrStruct kerr{ ControlDialog::worker.KerrData() };
				ControlDialog::worker.KerrChange(getCheck(IDC_KERR_HAS), kerr.kerr_mass, kerr.kerr_a_rate,
					(kerr.kerr_position[0] * NGlobal::scale_distance), (kerr.kerr_position[1] * NGlobal::scale_distance), (kerr.kerr_position[2] * NGlobal::scale_distance),
					getCheck(IDC_DISK_HAS), kerr.disk_speed);
			} break;
			//light
			case IDC_LIGHT:
			{
				ControlDialog::worker.LightSetIs(getCheck(IDC_LIGHT));
			} break;
			//sky
			case IDC_SKYBOX:
			{
				ControlDialog::worker.SkyChange(getCheck(IDC_SKYBOX), ControlDialog::worker.SkyPath(), ControlDialog::worker.SkyDensity());
			} break;
			case IDC_SKYBOX_FILE:
			{
				std::string path{};
				if (NCommonDialog::OpenFileDialog(path, std::vector<COMDLG_FILTERSPEC>
				{
					COMDLG_FILTERSPEC{ L"image file", L"*.png" },
					COMDLG_FILTERSPEC{ L"image file", L"*.jpg" },
					COMDLG_FILTERSPEC{ L"image file", L"*.jpeg" },
					COMDLG_FILTERSPEC{ L"image file", L"*.bmp" }
				}))
				{
					ControlDialog::worker.SkyChange(ControlDialog::worker.SkyHas(), path, ControlDialog::worker.SkyDensity());
				}
			} break;
			case IDC_SKYBOX_RANDOM:
			{
				ControlDialog::worker.SkyChange(ControlDialog::worker.SkyHas(), "", ControlDialog::worker.SkyDensity());
			} break;
			//apply
			case IDC_APPLY:
			{
				ControlDialog::Apply();
			} break;
			case IDC_RUN:
			{
				ControlDialog::worker.Run();
			} break;
			case IDC_STOP:
			{
				ControlDialog::worker.Stop();
			} break;
			}
		}
	}

protected:
	INT_PTR DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		switch (uMsg)
		{
		case WM_INITDIALOG:
		{
			ControlDialog::Init();
		} break;
		case WM_CLOSE:
		{
			if ((ControlDialog::editChange == ControlDialog::editInit) || (IDOK == ::MessageBoxA(IDialog::hDlg, "变更未应用，是否关闭?", "控制面板", (MB_OKCANCEL | MB_DEFBUTTON2))))
			{
				IDialog::EndDialog(0LL);
			}
		} break;
		case WM_COMMAND:
		{
			ControlDialog::Command(uMsg, wParam, lParam);
		} break;
		}
		return 0LL;
	}

public:
	ControlDialog(IWinUI* pWindow, Worker& worker) :
		IDialog{ IDD_CONTROLDIALOG },
		pWindow{ pWindow }, worker{ worker },
		editInit {}, editChange{}, chooseId{ -1 }
	{}
};

#endif // !_CONTROLDIALOG_H_