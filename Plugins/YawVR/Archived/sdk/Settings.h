#pragma once
/*
class Settings //: YawControllerDelegate
{
public:
    //YAW device settings ui elements
    
    Text setupTitleLabel;
    
    GameObject deviceListScrollViewContent;
    
    GameObject DeviceListItemPrefab;
    
    Button connectButton;
    
    InputField ipAddressInputField;
    
    InputField udpPortInputField;
    
    InputField tcpPortInputField;
    
    Button disconnectButton;
    
    Toggle rememberDeviceToggle;
    
    Text errorText;

    //Source of motion settings ui elements
    
    Dropdown sourceOfMotionDropdown;
    
    InputField yawRotationMultiplierInputField;
    
    InputField pitchRotationMultiplierInputField;
    
    InputField rollRotationMultiplierInputField;
    
    InputField pitchAccelerationMultiplierInputField;
    
    InputField rollAccelerationMultiplierInputField;
    
    InputField lateralForceMultiplierInputField;

    //Rotation limits settings ui elements
    
    InputField yawLimitInputField;
    
    InputField pitchForwardLimitInputField;
    
    InputField pitchBackwardLimitInputField;
    
    InputField rollLimitInputField;

    //DeviceDiscovery deviceDiscovery = new DeviceDiscovery();
    int? udpPort = 50010;
    int? tcpPort;

    IPAddress ipAddress;
    YawDevice selectedDevice;
    List<YawDevice> availableDevices = new List<YawDevice>();
    List<GameObject> deviceButtons = new List<GameObject>();

    bool firstEnable  = true;



    Settings(){};

    void Start();

    void OnDisable();

    void OnEnable();

    void OnDestroy();

    IEnumerator SearchForDevices();

    public void LayoutDeviceButtons(List<YawDevice> devices);

    void UDPPortInputFieldTextDidChange(InputField inputField);

    void TCPPortInputFieldTextDidChange(InputField inputField);

    void IPAddressInputFieldTextDidChange(InputField inputField);

    void SetDeviceFromPortAndIp();

    void ConnectButtonPressed();

    void DisconnectButtonPressed();

    void DeviceListItemPressed(YawDevice device);

    void SourceOfMotionDropDownValueChanged(Dropdown dropdown);

    void RotationMultiplierInputFieldTextDidChange(InputField inputField);

    void AccelerationMultiplierInputFieldTextDidChange(InputField inputField);

    void LateralForceMultiplierInputFieldTextDidChange(InputField inputField);


    void RotationLimitInputFieldTextDidChange(InputField inputField);

    void DidFoundDevice(YawDevice device);

    bool SameDevice(YawDevice device, YawDevice toDevice);

    void YawLimitDidChange(int currentLimit);

    void TiltLimitsDidChange(int pitchFrontLimit, int pitchBackLimit, int rollLimit);

    void RememberDeviceToggleValueDidChange(Toggle toggle);

    void DidDisconnectFrom(YawDevice device);

    void ControllerStateChanged(ControllerState state);

    void RefreshLayout(ControllerState state);

    void ShowError(string error, int duration = 10);

    IEnumerator ClearError(int duration);

}
*/