#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime
#  Résumé:
#  Etude statistique des performance d'éxecution d'un programme de multiplication matrice/vecteur.

library(ggplot2)
library(plyr)
library(reshape2)

# extraction des données
d <- data.frame(
    read.csv2(file="./Mesures/mult_mat_1_16.csv",
              sep=';',
              dec='.')
)

# filtrer les résultats pour AlgoType 1
filter_Algotype <- subset(d, Algotype==2)

# Temps moyen, écart type, variance
stat_raw<-ddply(
    d,
    c("Algotype", "Chunksize", "Nb_Thread"),
    summarise,
    N=length(Time),
    mean=mean(Time),
    sd=sd(Time),
    se=1.96*(sd/sqrt(N))
)

# tracé des résultats obtenu

p1 <- ggplot(data = stat_raw, mapping = aes(x=Nb_Thread, y=mean, color = as.factor(Chunksize))) +
    geom_point() +
    geom_line() +
    geom_errorbar(aes(ymin = mean-se, ymax = mean+se, width=0.5, fatten = 0.5 )) +
    scale_x_continuous(breaks = seq(1,32)) +
    #scale_y_log10() + 
    labs(
        colour = "Chunk Size",
        x = "Nombre de threads",
        y = "Temps moyen d'execution (en cycles) pour 1000 essais"
    ) + facet_wrap(~Algotype) +
        labs(
        fill = "Nombre Threads"
        ) +
    theme_minimal()
ggsave("./Images/Matrix_mult_facet_.png", plot = p1, scale = 0.1)

show(p1)
