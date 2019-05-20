Ensimag 1A 2016-17 - Projet C Tests décodeur JPEG


=== Niveaux de gris ============================================================
invader     1x1         8x8         1 seul bloc
gris        1x1         320x320     40x40 blocs, pas de blocs incomplets
bisou       1x1         585x487     blocs incomplets
albert      1x1         441x606     blocs incomplets        CHOISIR BISOU OU ALBERT?

complexite  1x1         2995x2319   pour exploser l'idct naive... (même en niveaux de gris)


=== Couleurs ===================================================================
zig-zag     1x1,1x1,1x1 480x680     pas de blocs incomplets, pas d'échantillonnage
thumbs      1x1,1x1,1x1 439x324     blocs incomplets, pas d'échantillonnage

vertical    1x2,1x1,1x1 704x1246    échantillonnage 4:2:2 vertical
horizontal  2x1,1x1,1x1 367x367     échantillonnage 4:2:2 horizontal

shaun:      2x2,1x1,1x1 300x225	    échantillonnage 4:2:0 vertical et horizontal



*** Toutes ces images ne sont pas libres de droits (carrément pas).
Mais on les aime bien, alors au moins faisons leur de la pub: lisez Spirou et
Lucky Luke, parlez l'anglais et l'américain, revoyez Les Temps Modernes, Bêêhh,
aimez la physique (à défaut de comprendre), jouez à des jeux d'hier et d'aujourd'hui,
et bécotez-vous devant l'hôtel de ville!

L'image "vertical" traduit l'émotion d'un(e) étudiant(e) qui vient d'arriver à finir l'upsampling.
L'image "horizontal" traduit la réaction de ses enseignants.
Le/la premièr(e) qui nous dit de quel album proviennent ces deux images gagne un Carambar! (il y a un piège)
