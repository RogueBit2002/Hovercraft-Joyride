#pragma once

#include "YawVR.h"

#include "SettingsButton.h"


void SettingsButton::Start()
{
    button = gameObject.GetComponent<Button>();

    button.onClick.AddListener(SettingsButtonPressed);
}

void SettingsButton::OnDestroy()
{
    button.onClick.RemoveAllListeners();
}

void SettingsButton::SettingsButtonPressed()
{
    if (!settingsPanel.activeInHierarchy)
    {
        settingsPanel.SetActive(true);
        if (YawController.Instance().State == ControllerState.Started) {
            //YAWController.instance.StopDevice();
            YawController.Instance().StopDevice(
                () => { 
            }, 
                (error) => { 
            });
        }
    }
    else
    {
        settingsPanel.SetActive(false);
        if (YawController.Instance().State == ControllerState.Connected) {
            //YAWController.instance.StartDevice();
            YawController.Instance().StartDevice(
                () => {
            },
                (error) => {
                });
        }

    }
}
