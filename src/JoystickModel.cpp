#include "../include/JoystickModel.h"

JoystickModel::JoystickModel(int x, int y)
    : x(x), y(y) {}

JoystickModel::JoystickModel(JoystickModel *joystickModel)
{
    if (joystickModel)
    {
        x = joystickModel->x;
        y = joystickModel->y;
    }
    else
    {
        x = 0;
        y = 0;
    }
}
bool JoystickModel::operator==(const JoystickModel &other) const
{
    if (x >= other.x - roomForManeuver && x <= other.x + roomForManeuver &&
        y >= other.y - roomForManeuver && y <= other.y + roomForManeuver)
    {
        return true;
    }
    return false;
}
JoystickModel::JoystickModel() : x(0), y(0) {};
cJSON* JoystickModel::toJson() const
{
    cJSON* json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "x", x);
    cJSON_AddNumberToObject(json, "y", y);
    return json;
}
JoystickModel JoystickModel::fromJson(cJSON* json)
{
    int _x = cJSON_GetObjectItemCaseSensitive(json, "x")->valueint;
    int _y = cJSON_GetObjectItemCaseSensitive(json, "y")->valueint;
    return JoystickModel(_x, _y);
};