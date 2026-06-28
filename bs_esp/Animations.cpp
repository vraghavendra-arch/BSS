#include "Animations.h"

bool animationRunning = false;
uint32_t animationLast = 0;
uint16_t animationStep = 0;

///////////////////////////////////////////////////////////
// NORTHERN LIGHTS
///////////////////////////////////////////////////////////

const uint8_t northernLights[][3] =
{
    {127,255,0},
    {0,200,83},
    {0,191,165},
    {0,229,255},
    {41,121,255},
    {170,0,255}
};

const uint8_t (*activePalette)[3] = northernLights;
uint8_t paletteSize = 6;
uint8_t activePaletteSize = 0;

static uint8_t currentPaletteIndex = 0;
static uint32_t colorStartTime = 0;
static bool transitionPhase = false;

String animationModel = "";





///////////////////////////////////////////////////////////
// RAINBOW
///////////////////////////////////////////////////////////

const uint8_t rainbow[][3] =
{
    {255,0,0},
    {255,127,0},
    {255,255,0},
    {0,255,0},
    {0,0,255},
    {75,0,130},
    {148,0,211}
};

///////////////////////////////////////////////////////////
// LOVE
///////////////////////////////////////////////////////////

const uint8_t love[][3] =
{
    {255,0,60},
    {255,20,147},
    {255,105,180},
    {255,182,193},
    {255,0,100}
};

///////////////////////////////////////////////////////////
// CANDLE
///////////////////////////////////////////////////////////

const uint8_t candle[][3] =
{
    {255,140,0},
    {255,180,60},
    {255,220,120},
    {255,170,40}
};

///////////////////////////////////////////////////////////
// SUNSET
///////////////////////////////////////////////////////////

const uint8_t sunset[][3] =
{
    {255,94,0},
    {255,120,0},
    {255,60,60},
    {255,0,120},
    {120,0,255}
};

///////////////////////////////////////////////////////////
// OCEAN
///////////////////////////////////////////////////////////

const uint8_t ocean[][3] =
{
    {0,40,255},
    {0,120,255},
    {0,200,255},
    {0,255,255},
    {0,100,180}
};

///////////////////////////////////////////////////////////
// FOREST
///////////////////////////////////////////////////////////

const uint8_t forest[][3] =
{
    {0,100,0},
    {0,180,0},
    {34,139,34},
    {50,205,50},
    {144,238,144}
};

///////////////////////////////////////////////////////////
// ICE
///////////////////////////////////////////////////////////

const uint8_t ice[][3] =
{
    {220,255,255},
    {180,240,255},
    {120,220,255},
    {80,180,255},
    {0,120,255}
};

///////////////////////////////////////////////////////////
// LAVA
///////////////////////////////////////////////////////////

const uint8_t lava[][3] =
{
    {255,0,0},
    {255,40,0},
    {255,80,0},
    {255,140,0},
    {255,255,0}
};

///////////////////////////////////////////////////////////
// GALAXY
///////////////////////////////////////////////////////////

const uint8_t galaxy[][3] =
{
    {20,0,80},
    {80,0,255},
    {180,0,255},
    {255,0,180},
    {0,180,255}
};

///////////////////////////////////////////////////////////
// FIREPLACE
///////////////////////////////////////////////////////////

const uint8_t fireplace[][3] =
{
    {255,40,0},
    {255,80,0},
    {255,120,0},
    {255,200,50},
    {255,100,0}
};

///////////////////////////////////////////////////////////
// CYBERPUNK
///////////////////////////////////////////////////////////

const uint8_t cyberpunk[][3] =
{
    {255,0,255},
    {180,0,255},
    {0,255,255},
    {0,150,255},
    {255,0,180}
};

///////////////////////////////////////////////////////////
// CINEMATIC
///////////////////////////////////////////////////////////

const uint8_t cinematic[][3] =
{
    {255,191,0},
    {255,140,0},
    {255,80,0},
    {120,40,255},
    {0,120,255}
};

///////////////////////////////////////////////////////////
// ZEN
///////////////////////////////////////////////////////////

const uint8_t zen[][3] =
{
    {255,220,180},
    {255,240,220},
    {220,255,220},
    {200,240,255}
};

///////////////////////////////////////////////////////////
// PARTY
///////////////////////////////////////////////////////////

const uint8_t party[][3] =
{
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {255,255,0},
    {255,0,255},
    {0,255,255}
};


void blendColor(
    uint8_t r1,uint8_t g1,uint8_t b1,
    uint8_t r2,uint8_t g2,uint8_t b2,
    float t,
    uint8_t &r,
    uint8_t &g,
    uint8_t &b)
{
    r = r1 + ((r2-r1)*t);
    g = g1 + ((g2-g1)*t);
    b = b1 + ((b2-b1)*t);
}

void initAnimation()
{
    animationRunning = false;
}

void startAnimation(const String& model)
{
    animationModel = model;

    currentPaletteIndex = 0;
    colorStartTime = millis();
    transitionPhase = false;

    animationStep = 0;
    animationRunning = true;

    activePalette = northernLights;
    activePaletteSize = sizeof(northernLights)/sizeof(northernLights[0]);

    if(model == "northernlights")
    {
        activePalette = northernLights;
        activePaletteSize = sizeof(northernLights)/sizeof(northernLights[0]);
    }
    else if(model == "rainbow")
    {
        activePalette = rainbow;
        activePaletteSize = sizeof(rainbow)/sizeof(rainbow[0]);
    }
    else if(model == "love")
    {
        activePalette = love;
        activePaletteSize = sizeof(love)/sizeof(love[0]);
    }
    else if(model == "candle")
    {
        activePalette = candle;
        activePaletteSize = sizeof(candle)/sizeof(candle[0]);
    }
    else if(model == "sunset")
    {
        activePalette = sunset;
        activePaletteSize = sizeof(sunset)/sizeof(sunset[0]);
    }
    else if(model == "ocean")
    {
        activePalette = ocean;
        activePaletteSize = sizeof(ocean)/sizeof(ocean[0]);
    }
    else if(model == "forest")
    {
        activePalette = forest;
        activePaletteSize = sizeof(forest)/sizeof(forest[0]);
    }
    else if(model == "ice")
    {
        activePalette = ice;
        activePaletteSize = sizeof(ice)/sizeof(ice[0]);
    }
    else if(model == "lava")
    {
        activePalette = lava;
        activePaletteSize = sizeof(lava)/sizeof(lava[0]);
    }
    else if(model == "galaxy")
    {
        activePalette = galaxy;
        activePaletteSize = sizeof(galaxy)/sizeof(galaxy[0]);
    }
    else if(model == "fireplace")
    {
        activePalette = fireplace;
        activePaletteSize = sizeof(fireplace)/sizeof(fireplace[0]);
    }
    else if(model == "cyberpunk")
    {
        activePalette = cyberpunk;
        activePaletteSize = sizeof(cyberpunk)/sizeof(cyberpunk[0]);
    }
    else if(model == "cinematic")
    {
        activePalette = cinematic;
        activePaletteSize = sizeof(cinematic)/sizeof(cinematic[0]);
    }
    else if(model == "zen")
    {
        activePalette = zen;
        activePaletteSize = sizeof(zen)/sizeof(zen[0]);
    }
    else if(model == "party")
    {
        activePalette = party;
        activePaletteSize = sizeof(party)/sizeof(party[0]);
    }

}

void stopAnimation()
{
    animationRunning = false;
}

bool isAnimationRunning()
{
    return animationRunning;
}

   void runAnimation()
    {
        if(!animationRunning)
            return;

        // static uint8_t currentPaletteIndex = 0;
        // static uint32_t colorStartTime = 0;
        // static bool transitionPhase = false;

        // const uint32_t HOLD_TIME = 3000;
        // const uint32_t FADE_TIME = 2000;

        // HOLD CURRENT COLOUR
        if(!transitionPhase)
        {
            targetColor[0] =
                activePalette[currentPaletteIndex][0];

            targetColor[1] =
                activePalette[currentPaletteIndex][1];

            targetColor[2] =
                activePalette[currentPaletteIndex][2];

            targetColor[3] = 0;

            if(millis() - colorStartTime > HOLD_TIME)
            {
                transitionPhase = true;
                colorStartTime = millis();
            }

            return;
        }

        // FADE TO NEXT COLOUR
        uint8_t nextIndex =
    (currentPaletteIndex + 1) %
    activePaletteSize;

        float t =
            (float)(millis() - colorStartTime)
            / FADE_TIME;

        if(t > 1.0f)
            t = 1.0f;

        uint8_t r,g,b;

        blendColor(
            activePalette[currentPaletteIndex][0],
            activePalette[currentPaletteIndex][1],
            activePalette[currentPaletteIndex][2],

            activePalette[nextIndex][0],
            activePalette[nextIndex][1],
            activePalette[nextIndex][2],

            t,

            r,g,b
        );

        targetColor[0] = r;
        targetColor[1] = g;
        targetColor[2] = b;
        targetColor[3] = 0;

        if(t >= 1.0f)
        {
            currentPaletteIndex = nextIndex;
            transitionPhase = false;
            colorStartTime = millis();
        }

        setSetting("anim_model", animationModel);
        setSetting("anim_index", String(currentPaletteIndex));
        setSetting("anim_phase", String(transitionPhase));
        saveSettings(false);
    }


    void restoreAnimation()
{
    animationModel =
        getSetting("anim_model", "");

    if(animationModel.length() == 0)
        return;

    currentPaletteIndex =
        getSetting("anim_index", "0").toInt();

    transitionPhase =
        getSetting("anim_phase", "0").toInt();

    startAnimation(animationModel);

    animationRunning = true;
}