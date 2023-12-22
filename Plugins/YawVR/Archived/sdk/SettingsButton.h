#pragma once

[RequireComponent(typeof(Button))]
class SettingsButton : MonoBehaviour
{
public:
    GameObject settingsPanel;

    Button button;

    SettingsButton() {};

    void OnDestroy();

    void Start();

    void SettingsButtonPressed();
};
