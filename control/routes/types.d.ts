import { FastifyInstance, RouteShorthandOptions } from 'fastify'

export interface Route { (fastify: FastifyInstance, opts: RouteShorthandOptions): Promise<void> }