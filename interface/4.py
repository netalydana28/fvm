#second step animation separate graphs one 
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk as NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from tkinter import *
import numpy as np
import scipy.interpolate
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

class interface:
    window = Tk()
    frame = Frame()

    def __init__(self):
        self.window.title("Navier Stokes by FVM")
        self.window.geometry("1536x864")
    
    def change_frame(self):
        self.entry_page.forget()
        self.main_page.pack()

    def read_data(self):
        f = open("out3M500K.dat", "r")
        self.X_data = []
        self.Y_data = []
        self.P_data = []
        self.U_data = []
        self.V_data = []
        self.C_data = []

        for x in f:
            list = [float(i) for i in x.split()]
            self.X_data.append(list[0])
            self.Y_data.append(list[1])
            self.P_data.append(list[2])
            self.U_data.append(list[3])
            self.V_data.append(list[4])
            self.C_data.append(list[5])

        f.close()
    
    def animate_P(self, i):
        self.p.clear()
        self.read_data()
        print(f"Pressure {self.P_data[0]}\n")
        rbf = scipy.interpolate.Rbf(self.X_data, self.Y_data, self.P_data, function = 'linear')
        pi = rbf(self.xi, self.yi)
        self.p.imshow(pi, vmin=min(self.P_data), vmax=max(self.P_data), origin = 'lower', extent=[min(self.X_data), max(self.X_data), min(self.Y_data), max(self.Y_data)])
        self.plot_p = self.p.scatter(self.X_data, self.Y_data, c=self.P_data, vmin=min(self.P_data), vmax=max(self.P_data))
        try: 
            self.cb_P.remove()
        except: 
            pass
        else:
            self.cb_P.remove()
        self.cb_P = plt.colorbar(self.plot_p, ax=self.p)
        # self.canvas_P.delete("all")
        self.canvas_P = FigureCanvasTkAgg(self.P, self.main_page)
        self.p.set_title("Pressure")
        self.canvas_P.draw()
        self.p.quiver(self.X_data, self.Y_data, self.U_data, self.V_data)
        self.canvas_P.get_tk_widget().grid(row=3,column=0)

    def animate_U(self, i):
        self.u.clear()
        rbf = scipy.interpolate.Rbf(self.X_data, self.Y_data, self.U_data, function = 'linear')
        ui = rbf(self.xi, self.yi)
        self.u.imshow(ui, vmin=min(self.U_data), vmax=max(self.U_data), origin = 'lower', extent=[min(self.X_data), max(self.X_data), min(self.Y_data), max(self.Y_data)])
        self.plot_u = self.u.scatter(self.X_data, self.Y_data, c=self.U_data, vmin=min(self.U_data), vmax=max(self.U_data))
        try: 
            self.cb_U.remove()
        except: 
            pass
        else:
            self.cb_U.remove()
        self.cb = plt.colorbar(self.plot_u, ax=self.u)
        # self.canvas_U.delete("all")
        self.canvas_U = FigureCanvasTkAgg(self.U, self.main_page)
        self.u.set_title("U-speed")
        self.canvas_U.draw()
        self.u.quiver(self.X_data, self.Y_data, self.U_data, self.V_data)
        self.canvas_U.get_tk_widget().grid(row=3,column=1)

    def animate_V(self, i):
        self.v.clear()
        rbf = scipy.interpolate.Rbf(self.X_data, self.Y_data, self.V_data, function = 'linear')
        vi = rbf(self.xi, self.yi)
        self.v.imshow(vi, vmin=min(self.V_data), vmax=max(self.V_data), origin = 'lower', extent=[min(self.X_data), max(self.X_data), min(self.Y_data), max(self.Y_data)])
        self.plot_v = self.v.scatter(self.X_data, self.Y_data, c=self.V_data, vmin=min(self.V_data), vmax=max(self.V_data))
        try: 
            self.cb_V.remove()
        except: 
            pass
        else:
            self.cb_V.remove()
        self.cb = plt.colorbar(self.plot_v, ax=self.v)
        # self.canvas_V.delete("all")
        self.canvas_V = FigureCanvasTkAgg(self.V, self.main_page)
        self.v.set_title("V-speed")
        self.canvas_V.draw()
        self.v.quiver(self.X_data, self.Y_data, self.U_data, self.V_data)
        self.canvas_V.get_tk_widget().grid(row=4,column=0)

    def animate_C(self, i):
        self.c.clear()
        rbf = scipy.interpolate.Rbf(self.X_data, self.Y_data, self.C_data, function = 'linear')
        ci = rbf(self.xi, self.yi)
        self.c.imshow(ci, vmin=min(self.C_data), vmax=max(self.C_data), origin = 'lower', extent=[min(self.X_data), max(self.X_data), min(self.Y_data), max(self.Y_data)])
        self.plot_c = self.c.scatter(self.X_data, self.Y_data, c=self.C_data, vmin=min(self.C_data), vmax=max(self.C_data))
        try: 
            self.cb_C.remove()
        except: 
            pass
        else:
            self.cb_C.remove()
        self.cb = plt.colorbar(self.plot_c, ax=self.c)
        # self.canvas_C.delete("all")
        self.canvas_C = FigureCanvasTkAgg(self.C, self.main_page)
        self.c.set_title("Concentration")
        self.canvas_C.draw()
        self.c.quiver(self.X_data, self.Y_data, self.U_data, self.V_data)
        self.canvas_C.get_tk_widget().grid(row=4,column=1)

    def main_frame(self):
        self.entry_page = Frame(self.window)
        self.main_page = Frame(self.window)
        self.entry_page.pack()
        self.read_data()
        #labels
        entry_title = Label(self.entry_page, text = "Solving Navier Stokes by FVM", font = ("Arial Bold" , 20))
        main_title = Label(self.main_page, text = "Change parameter", font=("Arial Bond", 12))
        Label(self.entry_page).grid(row=0, column=0)
        Label(self.main_page).grid(row=0, column=0)
        entry_title.grid(row=1, column=0)
        main_title.grid(row=1, column=0)
        #buttons 
        self.to_start = Button(self.entry_page, text="Start", font = ("Arial Bold", 12), command = self.change_frame).grid(row=2, column = 0)
        #setting up a regular grid of interpolation points
        self.xi, self.yi = np.linspace(max(self.X_data), min(self.X_data), 101), np.linspace(max(self.Y_data), min(self.Y_data), 101)
        self.xi, self.yi = np.meshgrid(self.xi, self.yi)
        #Pressure subplot
        self.P = Figure(figsize = (4,4), dpi = 100)
        self.p = self.P.add_subplot(111)
        #U-speed
        self.U = Figure(figsize = (4,4), dpi = 100)
        self.u = self.U.add_subplot(111)
        #V-speed
        self.V = Figure(figsize = (4,4), dpi = 100)
        self.v = self.V.add_subplot(111)
        #Concentration
        self.C = Figure(figsize = (4,4), dpi = 100)
        self.c = self.C.add_subplot(111)
        # app_P = SeaofBTCapp()
        # ani_P = animation.FuncAnimation(self.P, self.animate_P, interval=1000)
        # # app_P.mainloop()
        # # app_U = SeaofBTCapp()
        # ani_U = animation.FuncAnimation(self.U, self.animate_U, interval=1000)
        # # app_U.mainloop()
        # # app_V = SeaofBTCapp()
        # ani_V = animation.FuncAnimation(self.V, self.animate_V, interval=1000)
        # # app_V.mainloop()
        # ani_C = animation.FuncAnimation(self.C, self.animate_C, interval=1000)
        # app_C.mainloop()
        self.animate_P(1)
        self.animate_U(1)
        self.animate_V(1)
        self.animate_C(1)
        # app_P = SeaofBTCapp()
        ani_P = animation.FuncAnimation(self.P, self.animate_P, interval=100)
        # ani_P.save('pressure.mp4')
        plt.show()
        # # app_P.mainloop()
        # # app_U = SeaofBTCapp()
        ani_U = animation.FuncAnimation(self.U, self.animate_U, interval=1000)
        # ani_U.save('uspeed.mp4')
        # # app_U.mainloop()
        # # app_V = SeaofBTCapp()
        plt.show()
        ani_V = animation.FuncAnimation(self.V, self.animate_V, interval=1000)
        # ani_V.save('vspeed.mp4')
        # # app_V.mainloop()
        plt.show()
        ani_C = animation.FuncAnimation(self.C, self.animate_C, interval=1000)
        # ani_C.save('concentrarion.mp4')
        # app_C.mainloop()
        plt.show()

        self.window.mainloop()

if __name__ == "__main__":
    start = interface()
    start.main_frame()