import subprocess
import dash
import trafficC
from route_conf import *
from dash import dcc
from dash import html
from dash.dependencies import Output, Input
import dash_daq as daq
import pandas as pd
import trafficC
from os.path import exists
import numpy as np
import plotly.express as px

    
app = dash.Dash(__name__)

app.p = 0.2
app.p0 = 0.5
app.pchange = 0.2
app.pchange_slow = 0.4
app.psurr = 0.1
app.psurr_slow = 0.3
app.density = 2
app.gradient = 100


app.layout = html.Div(
    children=[
        html.H1(children="TrafficC - Simulador de Tráfico",),
        html.P(
            children="Elaboración: Miguel Ángel Uribe Laverde",
        ),
        html.P(
            children="Este software permite visualizar la evolución del tráfico bajo diferentes condiciones",
        ),
        html.Div(
            children = [
                html.Div(
                    children = [
                        html.H2(children="Parámetros de Movimiento de Autos",),
                        html.Div(
                            children = [
                                html.Div(
                                    children = [
                                        html.H3(children="P",),
                                        html.P(children="Probabilidad de frenado en movimiento",),
                                        daq.NumericInput(
                                            id='p_val',
                                            value=app.p,
                                            min = 0,
                                            max = 1,
                                            size = 80,
                                        )
                                    ]
                                ),
                                html.Div(
                                    children = [
                                        html.H3(children="P0",),
                                        html.P(children="Probabilidad de frenado en reposo",),
                                        daq.NumericInput(
                                            id='p0_val',
                                            value=app.p0,
                                            min = 0,
                                            max = 1,
                                            size = 80,
                                        )
                                    ]
                                )
                            ],
                            style={'width': '30%', 'display': 'inline-block'}
                        ),
                        html.Div(
                            children = [
                                html.Div(
                                    children = [
                                        html.H3(children="PChange",),
                                        html.P(children="Probabilidad de cambiar de carril a alta velocidad",),
                                        daq.NumericInput(
                                            id='pchange_val',
                                            value=app.pchange,
                                            min = 0,
                                            max = 1,
                                            size = 80,
                                        )
                                    ]
                                ),
                                html.Div(
                                    children = [
                                        html.H3(children="PChange_slow",),
                                        html.P(children="Probabilidad de cambiar de carril a baja velocidad",),
                                        daq.NumericInput(
                                            id='pchange_slow_val',
                                            value=app.pchange_slow,
                                            min = 0,
                                            max = 1,
                                            size = 80,
                                        )
                                    ]
                                )
                            ],
                            style={'width': '30%', 'display': 'inline-block'}
                        ),
                        html.Div(
                            children = [
                                html.Div(
                                    children = [
                                        html.H3(children="Psurr",),
                                        html.P(children="Probabilidad de cambiar de carril sin preocuparse por la seguridad, a altas velocidades",),
                                        daq.NumericInput(
                                            id='psurr_val',
                                            value=app.psurr,
                                            min = 0,
                                            max = 1,
                                            size = 80,
                                        )
                                    ]
                                ),
                                html.Div(
                                    children = [
                                        html.H3(children="PSurr_slow",),
                                        html.P(children="Probabilidad de cambiar de carril sin preocuparse por la seguridad, a bajas velocidades",),
                                        daq.NumericInput(
                                            id='psurr_slow_val',
                                            value=app.psurr_slow,
                                            min = 0,
                                            max = 1,
                                            size = 80,
                                        )
                                    ]
                                )
                            ],
                            style={'width': '30%', 'display': 'inline-block'}
                        )
                    ],
                    style={'width': '75%', 'display': 'inline-block'}
                ),
                html.Div(
                    children = [
                        html.H2(children="Parámetros de Tráfico",),
                        html.Div(
                            children = [
                                html.Div(
                                    children = [
                                        html.H3(children="Densidad",),
                                        html.P(children="El número promedio de autos por kilómetro de corredor",),
                                        daq.NumericInput(
                                            id='density_val',
                                            value=app.density,
                                            min = 0,
                                            max = 499,
                                            size = 80,
                                        )
                                    ],
                                ),
                                html.Div(
                                    children = [
                                        html.H3(children="Gradiente",),
                                        html.P(children="El gradiente de reducción de carriles",),
                                        daq.NumericInput(
                                            id='gradient_val',
                                            value=app.gradient,
                                            min = 0,
                                            max = 2000,
                                            size = 80,
                                        )
                                    ]
                                )
                            ]
                        )
                    ],
                    style={'width': '25%', 'display': 'inline-block'}
                )
            ]
        ),
        html.Button('Correr Simulacion', id='btn_simul', n_clicks=0, style={"margin": "15px"}),
        html.Div(
            children = [
                dcc.Graph(id="graph", style={'width': '75%', 'display': 'inline-block'}),
                html.Div(
                    children=[
                        html.Div(
                            children = [
                                html.H3(children="Flujo de autos"),
                                dcc.Markdown(id = "flujo_obs", mathjax=True),
                                html.H3(children="Velocidad promedio"),
                                dcc.Markdown(id = "vel_obs", mathjax=True),
                            ]
                        )
                        ],
                    style={'width': '25%', 'display': 'inline-block'})
            ]
        ),        
        html.Div(id='hidden-p', style={'display':'none'}),
        html.Div(id='hidden-p0', style={'display':'none'}),
        html.Div(id='hidden-pchange', style={'display':'none'}),
        html.Div(id='hidden-pchange_slow', style={'display':'none'}),
        html.Div(id='hidden-psurr', style={'display':'none'}),
        html.Div(id='hidden-psurr_slow', style={'display':'none'}),
        html.Div(id='hidden-density', style={'display':'none'}),
        html.Div(id='hidden-gradient', style={'display':'none'}),
        html.P(id='message'),
    ]
)

# callback to update the values 
@app.callback(
    Output('hidden-p', 'children'),
    Input('p_val', 'value')
)
def update_p(val):
    app.p = val
    return str(app.p)

# callback to update the values 
@app.callback(
    Output('hidden-p0', 'children'),
    Input('p0_val', 'value')
)
def update_p(val):
    #global p
    app.p0 = val
    return str(app.p0)

# callback to update the values 
@app.callback(
    Output('hidden-pchange', 'children'),
    Input('pchange_val', 'value')
)
def update_p(val):
    #global p
    app.pchange = val
    return str(app.pchange)

# callback to update the values 
@app.callback(
    Output('hidden-pchange_slow', 'children'),
    Input('pchange_slow_val', 'value')
)
def update_p(val):
    #global p
    app.pchange_slow = val
    return str(app.pchange_slow)

# callback to update the values 
@app.callback(
    Output('hidden-psurr', 'children'),
    Input('psurr_val', 'value')
)
def update_p(val):
    #global p
    app.psurr = val
    return str(app.psurr)

# callback to update the values 
@app.callback(
    Output('hidden-psurr_slow', 'children'),
    Input('psurr_slow_val', 'value')
)
def update_p(val):
    #global p
    app.psurr_slow = val
    return str(app.psurr_slow)

# callback to update the values 
@app.callback(
    Output('hidden-density', 'children'),
    Input('density_val', 'value')
)
def update_p(val):
    #global p
    app.density = val
    return str(app.density)

# callback to update the values 
@app.callback(
    Output('hidden-gradient', 'children'),
    Input('gradient_val', 'value')
)
def update_p(val):
    #global p
    app.gradient = val
    confname = trafficC.createSystemFiles(app.gradient)
    # the script is compiled when the executable does not exist
    if not exists('simulation_'+confname+'.exe'):
        print('Recompiling')
        comp = subprocess.run(['g++','-O2','simulation.cpp','-o','simulation_'+confname])
    return str(app.gradient)


# The simulation button
@app.callback(
    Output('message', 'children'),
    Output('flujo_obs', 'children'),
    Output('vel_obs', 'children'),
    Output('graph', 'figure'),
    Input('btn_simul', 'n_clicks'),
)
def displayClick(n):
    if n == 0:
        text = 'No button'
    else:
        text = 'running simulation for '+ str(app.p) + ' ' + str(app.p0)+ ' ' + str(app.pchange)+ ' ' + str(app.pchange_slow)+ ' ' + str(app.psurr)+ ' ' + str(app.psurr_slow)+ ' ' + str(app.density)+' ' + str(app.gradient)


    confname = trafficC.createSystemFiles(app.gradient)
    # we first check whether a simulation has been performed
    fileout = 'sim_results/sim_results_'+ confname+'_%d_%.2f_%.2f_%.2f_%.2f_%.2f_%.2f.txt'%(app.density, app.p, app.p0, app.pchange, app.pchange_slow, app.psurr, app.psurr_slow) 
    if not exists(fileout):
        trafficC.run_simulation_parallel(30,app.gradient, app.density, app.p, app.p0, app.pchange, app.pchange_slow, app.psurr, app.psurr_slow)

    # reading the results
    results = np.loadtxt(fileout)
    dflow = results[1]*3600
    fpow = np.floor(np.log10(dflow))
    dflow = np.round(results[1]*3600/10**fpow,0)*10**fpow
    flow = np.round(results[0]*3600/10**fpow,0)*10**fpow
    dspeed = results[3]*3.6
    vpow = np.floor(np.log10(dspeed))
    dspeed = np.round(results[3]*3.6/10**vpow,0)*10**vpow
    speed = np.round(results[2]*3.6/10**vpow,0)*10**vpow

    # loading the data
    Data = np.loadtxt(fileout[:-4]+'_cardata.txt')
    df = pd.DataFrame(data = Data, columns = ['time', 'x', 'y', 'ID'])
    df['color'] = df['ID']%10

    fig = px.scatter(df, x="x", y="y", animation_frame="time", 
            color="color", animation_group="ID", hover_name="ID", range_x=[xmin,xmax], range_y=[-0.5,5.5])


    return [text,'%d $$\pm$$ %d carros/min'%(flow, dflow), '%.1f $$\pm$$ %.1f km/h'%(speed, dspeed), fig]




if __name__ == "__main__":
    app.run_server(debug=True)