#!/usr/bin/env Rscript


#########################
# C S Cowden     10/7/2020
# This script creates a random miscalibration
# model to simulate the effects of miscalibration
# on a CNN.
#########################

library("optparse")
library("logger")
library("config")
library("VineCopula")
library("hdf5r")

#
# parse command line arguments
option_list <- list(
  make_option(c("-c","--config"), type="character", default="calib.cfg",
    help="Configuration script", metavar="character")
);

opt_parser <- OptionParser(option_list=option_list);
opt <- parse_args(opt_parser);


# fail out if no configuration is found
if ( !file.exists(opt$config) ) {
    print(paste(" FATAL ERROR: Could not open config file ",opt$config))
    stopifnot(file.exists(opt$config))
}

#
# open the config file
cfg <- config::get(file = opt$config );

#
# setup logging
if ( cfg$log_level == "TRACE" ) {
    log_threshold(TRACE);
} else if ( cfg$log_level == "DEBUG" ) {
    log_threshold(DEBUG);
} else if ( cfg$log_level == "INFO" ) {
    log_threshold(INFO);
} else if ( cfg$log_levl == "SUCCESS" ) {
    log_threshold(SUCCESS)
} else if ( cfg$log_level == "WARN" ) {
    log_threshold(WARN);
} else if ( cfg$log_level == "ERROR" ) {
    log_threshold(ERROR);
} else {
    log_threshold(FATAL);
}

log_appndr <- appender_console;
log_appender(log_appndr);

if ( !is.null(cfg$seed) ) {
    seed <- cfg$seed;
} else {
    seed <- 42;
}

#
# set the random seed (if passed)
set.seed(seed);
log_info(paste("Setting random seed to ",seed) );


#
# Given a detector's parameters, return the map of pixel to module and pixel to cells
create_pixel_map <- function(cell_shape,module_shape,pixel_shape) {
    # cell_shape is a 3 element list corresponding to the 2D shape of the cells and depth
    # module_shape is a 2 element list corresponding to the 2D shape of the module layout within a cell
    # pixel_shape is a 2 element list corresponding to the 2D shape of the pixel layout within a module
    
    nElements <- prod(cell_shape) * prod(module_shape) * prod(pixel_shape)
    
    element_shape <- c(cell_shape[1]*module_shape[1]*pixel_shape[1]
                       , cell_shape[2]*module_shape[2]*pixel_shape[2]
                       , cell_shape[3])
    
    
    #
    # create empty maps
    cell_map <- array(0,element_shape)
    module_map <- array(0,element_shape)
    pixel_map <- array(0,element_shape)
    pixel_label <- array(0,element_shape)
    
    
    #
    # assign maps value
    # assign map value by working through a queue of cells, modules, and pixels.
    i_pixprcell <- module_shape[1]*pixel_shape[1]
    j_pixprcell <- module_shape[2]*pixel_shape[2]
    label <- 1
    
    for ( k in 1:cell_shape[3] ){
        for ( i_cell in 1:cell_shape[1] ){
            for ( j_cell in 1:cell_shape[2] ) {
                for (i_mod in 1:module_shape[1] ){
                    for ( j_mod in 1:module_shape[2] ) {
                        for ( i_pix in 1:pixel_shape[1] ){
                            for (j_pix in 1:pixel_shape[2] ){
                                i <- (i_cell-1)*i_pixprcell + (i_mod-1)*pixel_shape[1] + i_pix
                                j <- (j_cell-1)*j_pixprcell + (j_mod-1)*pixel_shape[2] + j_pix
                                
                                cell_map[i,j,k] <- (i_cell-1)*cell_shape[1] + (k-1)*prod(cell_shape[1:2]) + j_cell
                                module_map[i,j,k] <- (i_mod-1)*module_shape[1] + j_mod
                                pixel_map[i,j,k] <- (i_pix-1)*pixel_shape[1] + j_pix
                                pixel_label[i,j,k] <- label
                                label <- label + 1
                                
                            }
                        }
                    }
                }
            }
        }
    }
    
    #
    # return the maps
    list(cell_map,module_map,pixel_map,pixel_label)
}


#
# given a set of geometry maps, and some high level correlation parameters
# built a vine model 
create_vine_model_m1 <- function(maps,imcor=0.75,omcor=0.) {
    # maps - list of maps returned by create_pixel_map
    
    # imcor - correlation within a given module
    
    # omcor - correlation between unrelated modules.
    
    mod_shape <- rep(max(maps[[4]]),2)
    mod_size <- prod(mod_shape)
    log_debug(paste('Model size:',mod_size))
    model <- matrix(0,mod_size)
    dim(model) <- mod_shape
    
    family <- matrix(0,mod_size)
    dim(family) <- mod_shape
    
    pars <- matrix(0,mod_size)
    dim(pars) <- mod_shape
    
    #
    # apply the module rules for dependence
    lo <- 1
    hi <- max(maps[[4]])
    log_debug('Applying d-vine rules to construct matrices')
    for ( k in 1:dim(model)[1] ) {
        
        # pick a low number, pick a high number
        if ( k %% 2 ) {
            # pick a low number
            model[k,k] <- lo
            lo <- lo + 1
            
            # get the module of the diagonal
            pixel <- which(maps[[4]] == model[k,k])
            pix_module <- maps[[2]][pixel]
            pix_cell <- maps[[1]][pixel]
            
            #count down from high
            dep <- hi
            j <- k+1
            while ( j <= mod_shape[1] ) {
                model[j,k] <- dep
                
                o_pix <- which(maps[[4]] == model[j,k])
                o_mod <- maps[[2]][o_pix]
                o_cell <- maps[[1]][o_pix]
                
                # apply proper copula and parameter
                if ( o_mod == pix_module && o_cell == pix_cell ) {
                    family[j,k] <- 1
                    pars[j,k] <- imcor
                } else if ( o_mod != pix_module && o_cell == pix_cell ) {
                    family[j,k] <- 1
                    pars[j,k] <- omcor
                } else {
                    family[j,k] <- 0
                }
                
                dep <- dep - 1
                j <- j + 1
            }
            
        } else {
            # pick a high number
            model[k,k] <- hi
            hi <- hi - 1
            
            # get the module of the diagonal
            pixel <- which(maps[[4]] == model[k,k])
            pix_module <- maps[[2]][pixel]
            pix_cell <- maps[[1]][pixel]
            
            # count up from lo
            dep <- lo
            j <- k+1
            while ( j <= mod_shape[1] ) {
                model[j,k] <- dep
                
                o_pix <- which(maps[[4]] == model[j,k])
                o_mod <- maps[[2]][o_pix]
                o_cell <- maps[[1]][o_pix]
                
                # apply the proper copula and paramters
                if ( o_mod == pix_module && o_cell == pix_cell ) {
                    family[j,k] <- 1
                    pars[j,k] <- rnorm(1,mean=imcor,sd=0.01)
                } else if ( o_mod != pix_module && o_cell == pix_cell ){
                    family[j,k] <- 1
                    pars[j,k] <- rnorm(1,mean=omcor,sd=0.01)
                } else {
                    family[j,k] <- 0
                }
                
                dep <- dep + 1
                j <- j + 1
            }
            
        }
        
    }
   
    log_debug('Returning model matrix, family matrix, and parameter matrix') 
    list(model,family,pars)
}


#
# create the maps
cell_shape <- c(cfg$cell_width, cfg$cell_height, cfg$cell_depth);
module_shape <- c(cfg$mod_width, cfg$mod_height);
pixel_shape <- c(cfg$pixel_width, cfg$pixel_height);

log_info(paste("cell shape:", paste(unlist(cell_shape), collapse=',')))
log_info(paste("modeul shape:", paste(unlist(module_shape), collapse=',')))
log_info(paste("pixel shape:", paste(unlist(pixel_shape), collapse=',')))
log_info("Creating pixel maps")

maps <- create_pixel_map(cell_shape, module_shape, pixel_shape);


#
# create the model
log_info("Creating vine matrices")
models <- create_vine_model_m1(maps, omcor=cfg$out_mod_cor, imcor=cfg$in_mod_cor);
log_debug('Calling RVineMatrix')
vine.matrix <- RVineMatrix(Matrix=models[[1]], family=models[[2]], par=models[[3]]);


#
# draw a calibration sample
log_info("Drawing calibration samples")
panels <- c(cfg$panel_width, cfg$panel_height, cfg$panel_depth)
num.panels <- prod(panels)
panel.size <- prod(dim(maps[[1]]))
draw.size <- cfg$num_draws

calib.raw.size <- prod(num.panels, panel.size, draw.size)

calibs <- vector(length=calib.raw.size)
calibs[] <- 0.
dim(calibs) <- c(num.panels, panel.size, draw.size)

for ( i in 1:num.panels ) {
    calibs[i,,] <- t(RVineSim(cfg$num_draws, vine.matrix));
    log_debug(sprintf("    Draw %d of %d complete.",i,num.panels));
}

#
# write the output
log_info(paste("Writing output to file", cfg$output))
h5.file <- H5File$new(cfg$output, mode="w")
h5.file$create_group("calib")
#h5.file[["calib/calib"]] <- array(unlist(calibs),dim=c(cfg$num_draws, unlist(dim(maps[[1]])), num.panels));
h5.file[["calib/calib"]] <- calibs
h5.file$close_all()


