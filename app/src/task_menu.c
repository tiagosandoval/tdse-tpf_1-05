/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file   : task_menu.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes */
#include "main.h"
#include <stdio.h>

/* Demo includes */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes */
#include "board.h"
#include "app.h"
#include "task_menu_attribute.h"
#include "task_menu_interface.h"
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_MEN_CNT_INI			0ul
#define G_TASK_MEN_TICK_CNT_INI		0ul

#define DEL_MEN_XX_MIN				0ul
#define DEL_MEN_XX_MED				50ul
#define DEL_MEN_XX_MAX				500ul

/********************** internal data declaration ****************************/
/* ADC1 fue creado por CubeMX en main.c */
extern ADC_HandleTypeDef hadc1;

/* Lectura instantánea del potenciómetro */
static uint32_t adc_value = 0;

/* Ángulo mostrado mientras se gira el potenciómetro */
static int32_t setpoint_preview = 0;

/* Ángulo guardado al confirmar con Enter */
static int32_t setpoint_angle = 0;

/* Controla cada cuánto se actualiza el LCD */
static uint32_t setpoint_update_counter = 0;

/* Datos compartidos con las tareas de IMU y servo. */
static shared_data_type *menu_shared_data = NULL;

task_menu_dta_t task_menu_dta =
    {DEL_MEN_XX_MIN, ST_MEN_MAIN_CONFIG, EV_MEN_ENT_IDLE, false};

#define MENU_DTA_QTY	(sizeof(task_menu_dta)/sizeof(task_menu_dta_t))

/********************** internal functions declaration ***********************/
void task_menu_statechart(void);
static void task_menu_display(task_menu_st_t state);
static void task_menu_write_lines(const char *line_1, const char *line_2);
static void task_menu_read_setpoint(void);
static void task_menu_display_setpoint(void);
static void task_menu_display_config_report(void);
static void task_menu_display_pid(void);
static void task_menu_display_angle(void);

/********************** internal data definition *****************************/
const char *p_task_menu 		= "Task Menu (Interactive Menu)";
const char *p_task_menu_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_menu_cnt;
volatile uint32_t g_task_menu_tick_cnt;

/********************** external functions definition ************************/
void task_menu_init(void *parameters)
{
	task_menu_dta_t *p_task_menu_dta;
	task_menu_st_t	state;
	task_menu_ev_t	event;
	bool b_event;

	menu_shared_data = (shared_data_type *)parameters;

	/* Print out: Task Initialized */
	LOGGER_INFO(" ");
	LOGGER_INFO("  %s is running - %s", GET_NAME(task_menu_init), p_task_menu);
	LOGGER_INFO("  %s is a %s", GET_NAME(task_menu), p_task_menu_);

	/* Init & Print out: Task execution counter */
	g_task_menu_cnt = G_TASK_MEN_CNT_INI;
	LOGGER_INFO("   %s = %lu", GET_NAME(g_task_menu_cnt), g_task_menu_cnt);

	init_queue_event_task_menu();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	/* Init & Print out: Task execution FSM */
	state = ST_MEN_MAIN_CONFIG;
	p_task_menu_dta->state = state;

	event = EV_MEN_ENT_IDLE;
	p_task_menu_dta->event = event;

	b_event = false;
	p_task_menu_dta->flag = b_event;

	LOGGER_INFO(" ");
	LOGGER_INFO("   %s = %lu   %s = %lu   %s = %s",
				 GET_NAME(state), (uint32_t)state,
				 GET_NAME(event), (uint32_t)event,
				 GET_NAME(b_event), (b_event ? "true" : "false"));

	/* Init LCD display and show the main menu. */
	displayInit( DISPLAY_CONNECTION_GPIO_4BITS );
	task_menu_display(state);
}

void task_menu_update(void *parameters)
{
	(void)parameters;
	bool b_time_update_required = false;

	/* Protect shared resource */
	__asm("CPSID i");	/* disable interrupts */
    if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
    {
		/* Update Tick Counter */
    	g_task_menu_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts */

    while (b_time_update_required)
    {
		/* Update Task Counter */
		g_task_menu_cnt++;

		/* Run Task Menu Statechart */
    	task_menu_statechart();

    	/*
    	 * Mientras estamos ingresando el Set Point, leer el
    	 * potenciómetro y actualizar el LCD aproximadamente
    	 * cada 100 ejecuciones de la tarea.
    	 */
		if ((ST_MEN_SETPOINT_VALUE == task_menu_dta.state) ||
		    (ST_MEN_REPORT_MODE == task_menu_dta.state) ||
		    (ST_MEN_PID_CONTROL == task_menu_dta.state) ||
		    (ST_MEN_ANGLE_REPORT == task_menu_dta.state))
		{
    	    setpoint_update_counter++;

    	    if (setpoint_update_counter >= 100)
    	    {
    	        setpoint_update_counter = 0;
		        if (ST_MEN_SETPOINT_VALUE == task_menu_dta.state)
		        {
		            task_menu_read_setpoint();
		            task_menu_display_setpoint();
		        }
		        else if (ST_MEN_REPORT_MODE == task_menu_dta.state)
		        {
		            task_menu_display_config_report();
		        }
		        else if (ST_MEN_PID_CONTROL == task_menu_dta.state)
		        {
		            task_menu_display_pid();
		        }
		        else
		        {
		            task_menu_display_angle();
		        }
    	    }
    	}
    	else
    	{
    	    setpoint_update_counter = 0;
    	}

    	/* Protect shared resource */
		__asm("CPSID i");	/* disable interrupts */
		if (G_TASK_MEN_TICK_CNT_INI < g_task_menu_tick_cnt)
		{
			/* Update Tick Counter */
			g_task_menu_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts */
	}
}

void task_menu_statechart(void)
{
	task_menu_dta_t *p_task_menu_dta;
	task_menu_st_t previous_state;

    /* Update Task Menu Data Pointer */
	p_task_menu_dta = &task_menu_dta;

	if (true == any_event_task_menu())
	{
		p_task_menu_dta->flag = true;
		p_task_menu_dta->event = get_event_task_menu();
	}
	else
	{
		return;
	}

	previous_state = p_task_menu_dta->state;

	switch (p_task_menu_dta->state)
	{
		case ST_MEN_MAIN_CONFIG:
			if (p_task_menu_dta->event == EV_MEN_NEX_ACTIVE) p_task_menu_dta->state = ST_MEN_MAIN_ACTION;
			else if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) p_task_menu_dta->state = ST_MEN_CONFIG_SETPOINT;
			break;
		case ST_MEN_MAIN_ACTION:
			if (p_task_menu_dta->event == EV_MEN_NEX_ACTIVE) p_task_menu_dta->state = ST_MEN_MAIN_CONFIG;
			else if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) p_task_menu_dta->state = ST_MEN_ACTION_CONTROL;
			break;
		case ST_MEN_CONFIG_SETPOINT:
			if (p_task_menu_dta->event == EV_MEN_NEX_ACTIVE) p_task_menu_dta->state = ST_MEN_CONFIG_REPORT;
			else if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) p_task_menu_dta->state = ST_MEN_SETPOINT_VALUE;
			else if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_MAIN_CONFIG;
			break;
		case ST_MEN_CONFIG_REPORT:
			if (p_task_menu_dta->event == EV_MEN_NEX_ACTIVE) p_task_menu_dta->state = ST_MEN_CONFIG_SETPOINT;
			else if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) p_task_menu_dta->state = ST_MEN_REPORT_MODE;
			else if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_MAIN_CONFIG;
			break;
		case ST_MEN_SETPOINT_VALUE:
			/*
			 * Se guarda provisoriamente el valor mostrado.
			 * La confirmación definitiva ocurre en la
			 * pantalla siguiente.
			 */
		    if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE)
		    {
		        setpoint_angle = setpoint_preview;
		        p_task_menu_dta->state = ST_MEN_SETPOINT_CONFIRM;
		    }
		    else if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE)
		    {
		        p_task_menu_dta->state = ST_MEN_CONFIG_SETPOINT;
		    }
		    break;

	case ST_MEN_SETPOINT_CONFIRM:
			if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE)
			{
				if (NULL != menu_shared_data)
					menu_shared_data->setpoint_deg = (float)setpoint_angle;
				p_task_menu_dta->state = ST_MEN_SETTINGS_SAVED;
			}
			else if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_SETPOINT_VALUE;
			break;
		case ST_MEN_SETTINGS_SAVED:
			if ((p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) || (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE)) p_task_menu_dta->state = ST_MEN_CONFIG_SETPOINT;
			break;
		case ST_MEN_REPORT_MODE:
			if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_CONFIG_REPORT;
			break;
		case ST_MEN_ACTION_CONTROL:
			if (p_task_menu_dta->event == EV_MEN_NEX_ACTIVE) p_task_menu_dta->state = ST_MEN_ACTION_REPORT;
			else if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) p_task_menu_dta->state = ST_MEN_PID_CONTROL;
			else if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_MAIN_ACTION;
			break;
		case ST_MEN_ACTION_REPORT:
			if (p_task_menu_dta->event == EV_MEN_NEX_ACTIVE) p_task_menu_dta->state = ST_MEN_ACTION_CONTROL;
			else if (p_task_menu_dta->event == EV_MEN_ENT_ACTIVE) p_task_menu_dta->state = ST_MEN_ANGLE_REPORT;
			else if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_MAIN_ACTION;
			break;
		case ST_MEN_PID_CONTROL:
			if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_ACTION_CONTROL;
			break;
		case ST_MEN_ANGLE_REPORT:
			if (p_task_menu_dta->event == EV_MEN_ESC_ACTIVE) p_task_menu_dta->state = ST_MEN_ACTION_REPORT;
			break;
		default:
			p_task_menu_dta->tick  = DEL_MEN_XX_MIN;
			p_task_menu_dta->state = ST_MEN_MAIN_CONFIG;
			p_task_menu_dta->event = EV_MEN_ENT_IDLE;
			break;
	}

	if (p_task_menu_dta->flag)
	{
		p_task_menu_dta->flag = false;
		if (NULL != menu_shared_data)
			menu_shared_data->pid_enabled =
				(ST_MEN_PID_CONTROL == p_task_menu_dta->state);
		if (previous_state != p_task_menu_dta->state)
			task_menu_display(p_task_menu_dta->state);
	}
}

static void task_menu_write_lines(const char *line_1, const char *line_2)
{
	displayCharPositionWrite(0, 0);
	displayStringWrite("                ");
	displayCharPositionWrite(0, 1);
	displayStringWrite("                ");
	displayCharPositionWrite(0, 0);
	displayStringWrite(line_1);
	displayCharPositionWrite(0, 1);
	displayStringWrite(line_2);
}

static void task_menu_display(task_menu_st_t state)
{
	switch (state)
	{
		case ST_MEN_MAIN_CONFIG:     task_menu_write_lines("MENU PRINCIPAL", "> Configuracion"); break;
		case ST_MEN_MAIN_ACTION:     task_menu_write_lines("MENU PRINCIPAL", "> Modo Accion"); break;
		case ST_MEN_CONFIG_SETPOINT: task_menu_write_lines("CONFIGURACION", "> Set Point"); break;
		case ST_MEN_CONFIG_REPORT:   task_menu_write_lines("CONFIGURACION", "> Modo reporte"); break;
		case ST_MEN_SETPOINT_VALUE:
		    task_menu_read_setpoint();
		    task_menu_display_setpoint();
		    break;
		case ST_MEN_SETPOINT_CONFIRM:task_menu_write_lines("Confirmar valor?", "ENT=Si ESC=No"); break;
		case ST_MEN_SETTINGS_SAVED:  task_menu_write_lines("Config guardada", "ENT/ESC volver"); break;
		case ST_MEN_REPORT_MODE:     task_menu_display_config_report(); break;
		case ST_MEN_ACTION_CONTROL:  task_menu_write_lines("MODO ACCION", "> Control PID"); break;
		case ST_MEN_ACTION_REPORT:   task_menu_write_lines("MODO ACCION", "> Reporte angulo"); break;
		case ST_MEN_PID_CONTROL:     task_menu_display_pid(); break;
		case ST_MEN_ANGLE_REPORT:    task_menu_display_angle(); break;
		default:                     task_menu_write_lines("MENU PRINCIPAL", "> Configuracion"); break;
	}
}

static void task_menu_read_setpoint(void)
{
    /*
     * Iniciar una conversión del ADC y esperar como máximo 10 ms.
     */
    if (HAL_OK == HAL_ADC_Start(&hadc1))
    {
        if (HAL_OK == HAL_ADC_PollForConversion(&hadc1, 10))
        {
            adc_value = HAL_ADC_GetValue(&hadc1);

            /*
             * Conversión:
             *
             * ADC = 0       -> -30 grados
             * ADC = 2048    ->   0 grados aproximadamente
             * ADC = 4095    -> +30 grados
             *
             * Se utiliza int32_t para poder representar
             * correctamente los valores negativos.
             */
            setpoint_preview =
                -30 + (int32_t)((adc_value * 60UL + 2047UL) / 4095UL);
        }

        HAL_ADC_Stop(&hadc1);
    }
}

static void task_menu_display_setpoint(void)
{
    char angle_text[17];

    /*
     * %+ld muestra siempre el signo:
     * -15, +0, +23, etc.
     */
    snprintf(angle_text,
             sizeof(angle_text),
             "Angulo: %+ld deg",
             (long)setpoint_preview);

    task_menu_write_lines("Set Point", angle_text);
}

static void task_menu_display_config_report(void)
{
    char value_text[17];
    long setpoint = (NULL != menu_shared_data) ?
                    (long)menu_shared_data->setpoint_deg : 0L;

    snprintf(value_text, sizeof(value_text), "SetPt: %+ld deg", setpoint);
    task_menu_write_lines("CONFIG GUARDADA", value_text);
}

static void task_menu_display_pid(void)
{
    char line_1[17];
    char line_2[17];
    long setpoint = 0L;
    long angle = 0L;

    if (NULL != menu_shared_data)
    {
        setpoint = (long)menu_shared_data->setpoint_deg;
        angle = (long)menu_shared_data->tita_deg;
    }

    snprintf(line_1, sizeof(line_1), "PID SP:%+ld deg", setpoint);
    snprintf(line_2, sizeof(line_2), "Actual:%+ld deg", angle);
    task_menu_write_lines(line_1, line_2);
}

static void task_menu_display_angle(void)
{
    char angle_text[17];

    if ((NULL == menu_shared_data) || !menu_shared_data->imu_ok)
    {
        task_menu_write_lines("Angulo actual", "IMU sin lectura");
        return;
    }

    snprintf(angle_text, sizeof(angle_text), "Valor: %+ld deg",
             (long)menu_shared_data->tita_deg);
    task_menu_write_lines("Angulo actual", angle_text);
}

/********************** end of file ******************************************/
