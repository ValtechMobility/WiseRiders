/*
 * Copyright (c) Microsoft
 * Copyright (c) 2024 Eclipse Foundation
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the MIT license which is available at
 *  https://opensource.org/license/mit.
 *
 *  SPDX-License-Identifier: MIT
 *
 *  Contributors:
 *     Microsoft         - Initial version
 *     Frédéric Desbiens - 2024 version.
 */

#include "screen.h"

#include "ssd1306.h"
#include "icon_font.h"

void screen_print(char *str, LINE_NUM line)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, line);
    ssd1306_WriteString(str, Font_11x18, White);
    ssd1306_UpdateScreen();
}

void screen_print_lines(char lines[][20], int num_lines, LINE_NUM line, int vert_spacing)
{
    ssd1306_Fill(Black);

    for (int i = 0; i < num_lines; i++)
    {
        ssd1306_SetCursor(2, line + (i * (FontIcon11x18.FontHeight + vert_spacing)));
        ssd1306_WriteString(lines[i], FontIcon11x18, White);
    }
    ssd1306_UpdateScreen();
}

void screen_printn(const char *str, unsigned int str_length, LINE_NUM line)
{
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, line);

    for (unsigned int i = 0; i < str_length; ++i)
    {
        if (ssd1306_WriteChar(str[i], Font_11x18, White) != str[i])
        {
            return;
        }
    }

    ssd1306_UpdateScreen();
}
