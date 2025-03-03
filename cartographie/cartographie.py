import numpy as np
import matplotlib.pyplot as plt
from matplotlib import animation
from scipy import interpolate 
import cartopy.crs as ccrs
from cartopy.io.shapereader import Reader
from cartopy.feature import ShapelyFeature

def interpolate_map(X, y, lats, lons, epsilon=500):
    # y (n,) : valeurs à interpoler
    # X (n, 2) : coordonnées des points de mesure
    # lons, lats (m, m) : coordonnées des points de la grille (meshgrid)
    # epsilon (float) : paramètre de régularisation pour l'interpolation
    # return grid (m, m) : valeurs interpolées
    grid = interpolate.RBFInterpolator(X, y, kernel="gaussian", epsilon=epsilon).__call__(np.array([lats.reshape(-1), lons.reshape(-1)]).T)
    grid = grid.reshape(lats.shape[0], lats.shape[1])
    return grid


def plot_map(lats, lons, grid, filename, vmin=None, vmax=None, timestamp=None, title="Temperature [°C]", cmap="coolwarm"):
    # lons, lats (m, m) : coordonnées des points de la grille (meshgrid)
    # data (m, m) : valeurs interpolées
    # filename (str) : nom du fichier de sauvegarde
    # vmin, vmax (float) : valeurs minimales et maximales pour la colormap (optionnel)
    # timestamp (str) : date et heure de la carte (optionnel)
    # title (str) : titre de la carte (optionnel)
    # cmap (str) : nom de la colormap (optionnel)
    if vmin is None:
        vmin = grid.min()
    if vmax is None:
        vmax = grid.max()
        
    fig, ax = plt.subplots(figsize=(10, 10),subplot_kw={'projection': ccrs.PlateCarree()})
    im = ax.contourf(lons, lats, grid, cmap=cmap, transform=ccrs.PlateCarree(), levels=np.linspace(vmin, vmax, 20))
    mask = ShapelyFeature(Reader("./QGIS/isae_masque.shp").geometries(),
                                    ccrs.PlateCarree(), facecolor='white', edgecolor='black')
    bats = ShapelyFeature(Reader("./QGIS/isae_batiments_final.shp").geometries(),
                                    ccrs.PlateCarree(), facecolor='white', edgecolor='black')
    ax.add_feature(mask)
    ax.add_feature(bats)
    gl = ax.gridlines(draw_labels=True)
    gl.top_labels = False
    gl.right_labels = False
    fig.colorbar(im, label=title)
    if timestamp is not None:
        ax.set_title(timestamp)

    fig.savefig(filename, bbox_inches="tight")
    
    
def plot_animation(X, y, lats, lons, filename, epsilon=500, dates=None, title="Temperature [°C]", cmap="coolwarm"):
    # y (n, p) : valeurs à interpoler: n points, p pas de temps
    # X (n, 2) : coordonnées des points de mesure
    # lats, lons (m, m) : coordonnées des points de la grille (meshgrid)
    # filename (str) : nom du fichier de sauvegarde
    # epsilon (float) : paramètre de régularisation pour l'interpolation
    # dates (n,) : dates associées à chaque pas de temps (optionnel)
    # title (str) : titre de la carte (optionnel)
    # cmap (str) : nom de la colormap (optionnel)
    fig, ax = plt.subplots(figsize=(10, 10),subplot_kw={'projection': ccrs.PlateCarree()})

    y0 = y[:, 0]
    data = interpolate_map(X, y0, lats, lons, epsilon=epsilon)
    vmin = y.min()
    vmax = y.max()
    im = ax.contourf(lons, lats, data, cmap=cmap, transform=ccrs.PlateCarree(), levels=np.linspace(vmin, vmax, 20))

    mask = ShapelyFeature(Reader("./QGIS/isae_masque.shp").geometries(),
                                    ccrs.PlateCarree(), facecolor='white', edgecolor='black')
    bats = ShapelyFeature(Reader("./QGIS/isae_batiments_final.shp").geometries(),
                                    ccrs.PlateCarree(), facecolor='white', edgecolor='black')
    ax.add_feature(mask)
    ax.add_feature(bats)
    gl = ax.gridlines(draw_labels=True)
    gl.top_labels = False
    gl.right_labels = False
    if dates is not None:
        ax.set_title(dates[0])
    else:
        ax.set_title("t = 0")
    fig.colorbar(im, label=title, extend="both")

    def update(frame):
        yf = y[:, frame]
        data = interpolate_map(X, yf, lats, lons)
        im = ax.contourf(lons, lats, data, cmap=cmap, transform=ccrs.PlateCarree(), levels=np.linspace(vmin, vmax, 20))
        if dates is not None:
            ax.set_title(dates[frame])
        else:
            ax.set_title("t = {}".format(frame))
        return im

    ani = animation.FuncAnimation(fig=fig, func=update, frames=y.shape[1], interval=1)
    ani.save(filename, writer="ffmpeg")
    
    
if __name__ == "__main__":
    
    print("Test des fonctions de cartographie à partir de fausses données...")
    
    # génération des données
    serie_1 = np.sin(np.linspace(0, 2*np.pi, 50)) 
    serie_2 = np.sin(np.linspace(0, 2*np.pi, 50) + np.pi/4)
    serie_3 = np.sin(np.linspace(0, 2*np.pi, 50) + np.pi/2)

    lat_1, lon_1 = 43.57, 1.471
    lat_2, lon_2 = 43.564, 1.4755
    lat_3, lon_3 = 43.5655, 1.477

    y = np.array([serie_1, serie_2, serie_3])
    X = np.array([[lat_1, lon_1], [lat_2, lon_2], [lat_3, lon_3]])

    # interpolation
    y0 = y[:, 0]
    
    lats = np.linspace(43.563, 43.5735, 100)
    lons = np.linspace(1.470, 1.4785, 100)
    lons, lats = np.meshgrid(lons, lats)
    
    # interpolation
    data0 = interpolate_map(X, y0, lats, lons)
    
    # carte unique
    plot_map(lats, lons, data0, "./test.png", vmin=y.min(), vmax=y.max())
    
    # carte animée
    plot_animation(X, y, lats, lons, "./test.gif")