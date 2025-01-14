<?xml version="1.0" encoding="utf-8" ?>
<ResourceDictionary xmlns="http://schemas.microsoft.com/dotnet/2021/maui"
                    xmlns:x="http://schemas.microsoft.com/winfx/2009/xaml">

    <!-- Common Styles -->
    <Style TargetType="Page">
        <Setter Property="BackgroundColor" Value="{StaticResource BackgroundColor}" />
    </Style>

    <Style TargetType="Label">
        <Setter Property="TextColor" Value="{StaticResource TextColor}" />
    </Style>

    <Style TargetType="Button">
        <Setter Property="BackgroundColor" Value="{StaticResource PrimaryColor}" />
        <Setter Property="TextColor" Value="{StaticResource TextColor}" />
        <Setter Property="Padding" Value="15,10" />
        <Setter Property="CornerRadius" Value="5" />
    </Style>

    <Style TargetType="Frame">
        <Setter Property="BackgroundColor" Value="{StaticResource SurfaceColor}" />
        <Setter Property="BorderColor" Value="{StaticResource SecondaryColor}" />
        <Setter Property="CornerRadius" Value="10" />
        <Setter Property="Padding" Value="15" />
    </Style>

    <!-- Settings Page Styles -->
    <Style x:Key="SettingsGroupFrame" TargetType="Frame">
        <Setter Property="BackgroundColor" Value="{StaticResource SurfaceColor}" />
        <Setter Property="BorderColor" Value="{StaticResource SecondaryColor}" />
        <Setter Property="CornerRadius" Value="10" />
        <Setter Property="Padding" Value="20" />
        <Setter Property="HasShadow" Value="True" />
    </Style>

    <Style x:Key="SettingsGroupHeader" TargetType="Label">
        <Setter Property="TextColor" Value="{StaticResource TextColor}" />
        <Setter Property="FontSize" Value="18" />
        <Setter Property="FontAttributes" Value="Bold" />
        <Setter Property="Margin" Value="0,0,0,10" />
    </Style>
</ResourceDictionary>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               